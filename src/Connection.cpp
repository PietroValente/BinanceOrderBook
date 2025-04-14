#include <iostream>
#include <sstream>
#include <ws2tcpip.h>

#include "Connection.h"

Connection::Connection(const std::string& host, const std::string& port)
    : host(host), port(port), ctx(nullptr), ssl(nullptr), sockfd(INVALID_SOCKET), server(nullptr) {}

Connection::~Connection() {
    cleanup();
}

void Connection::cleanup() {
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    if (sockfd != INVALID_SOCKET) {
        closesocket(sockfd);
    }
    WSACleanup();
    if (ctx) {
        SSL_CTX_free(ctx);
    }
}

bool Connection::setup() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD* method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) return false;

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "socket failed\n";
        return false;
    }

    struct addrinfo hints = {};
    struct addrinfo* result = nullptr;
    hints.ai_family = AF_INET; // IPv4 only
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int res = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
    if (res != 0 || result == nullptr) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(res) << "\n";
        return false;
    }

    res = connect(sockfd, result->ai_addr, static_cast<int>(result->ai_addrlen));
    freeaddrinfo(result);
    if (res != 0) {
        std::cerr << "connect failed\n";
        return false;
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, static_cast<int>(sockfd));
    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL_connect failed\n";
        return false;
    }
    return true;
}

std::string Connection::getJson(const std::string& path) {
    std::ostringstream request;
    request << "GET " << path << " HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "Connection: keep-alive\r\n";
    request << "\r\n";

    if (SSL_write(ssl, request.str().c_str(), static_cast<int>(request.str().length())) <= 0) {
        std::cerr << "Failed to send the request." << std::endl;
        return "";
    }

    int bytesRead = 0;
    int totalBytesRead = 0;
    size_t contentLength = -1;
    char buffer[4096];
    std::string payload;
    bool foundHeaderEnd = false;

    while ((bytesRead = SSL_read(ssl, buffer, sizeof(buffer))) > 0) {
        totalBytesRead += bytesRead;

        if (!foundHeaderEnd) {
            size_t headerEndPos = totalBytesRead;
            for (size_t i = 0; i < totalBytesRead - 3; i++) {
                if (buffer[i] == '\r' && buffer[i + 1] == '\n' && buffer[i + 2] == '\r' && buffer[i + 3] == '\n') {
                    headerEndPos = i + 4;
                    foundHeaderEnd = true;
                    break;
                }
            }

            if (foundHeaderEnd) {
                for (size_t i = 0; i < headerEndPos - 4; i++) {
                    if (strstr(buffer + i, "Content-Length: ") == buffer + i) {
                        contentLength = atoi(buffer + i + 15);
                        break;
                    }
                }

                if (contentLength == -1) {
                    std::cerr << "Content-Length header missing." << std::endl;
                    return "";
                }
                
                payload.append(buffer + headerEndPos, bytesRead - headerEndPos);
            }
        } else {
            payload.append(buffer, bytesRead);
        }

        if (payload.length() >= contentLength) {
            break;
        }
    }

    return payload;
}
