#include <iostream>
#include <sstream>

#include "Connection.h"

Connection::Connection(const std::string& host, const std::string& port)
    : host(host), port(port), ctx(nullptr), ssl(nullptr), sockfd(INVALID_SOCKET_VAL), server(nullptr) {}

Connection::~Connection() {
    cleanup();
}

void Connection::cleanup() {
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    if (sockfd != INVALID_SOCKET_VAL) {
        CLOSE_SOCKET(sockfd);
    }
    SOCKET_CLEANUP();
    if (ctx) {
        SSL_CTX_free(ctx);
    }
}

bool Connection::setup() {
#ifdef _WIN32
    WSADATA wsaData;
#endif
    if (SOCKET_INIT() != 0) {
        std::cerr << "Socket initialization failed\n";
        return false;
    }

    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD* method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        std::cerr << "Failed to create SSL_CTX\n";
        return false;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET_VAL) {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    struct addrinfo hints {};
    struct addrinfo* result = nullptr;
    hints.ai_family = AF_INET;
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
        std::cerr << "Connect failed\n";
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

std::string Connection::getPayload(const std::string& path) {
    std::ostringstream request;
    request << "GET " << path << " HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "Connection: keep-alive\r\n";
    request << "\r\n";

    if (SSL_write(ssl, request.str().c_str(), static_cast<int>(request.str().length())) <= 0) {
        std::cerr << "Failed to send the request." << std::endl;
        return "";
    }

    char buffer[4096];
    std::string payload;
    bool foundHeaderEnd = false;
    size_t contentLength = -1;

    while (true) {
        int bytesRead = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytesRead <= 0) break;

        if (!foundHeaderEnd) {
            std::string headers(buffer, bytesRead);
            size_t pos = headers.find("\r\n\r\n");
            if (pos != std::string::npos) {
                foundHeaderEnd = true;

                size_t clPos = headers.find("Content-Length: ");
                if (clPos != std::string::npos) {
                    contentLength = std::stoi(headers.substr(clPos + 16));
                } else {
                    std::cerr << "Content-Length header missing." << std::endl;
                    return "";
                }

                std::cout << buffer;
                size_t bodyStart = pos + 4;
                payload += headers.substr(bodyStart);
            }
        } else {
            payload.append(buffer, bytesRead);
        }

        if (contentLength != -1 && payload.length() >= contentLength) {
            break;
        }
    }

    return payload;
}
