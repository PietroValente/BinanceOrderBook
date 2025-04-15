#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "Connection.h"

Connection::Connection(const std::string& host, const std::string& port, const std::string& symbol)
    : host(host), port(port), symbol(symbol), ctx(nullptr), ssl(nullptr), sockfd(INVALID_SOCKET_VAL) {
        subscribeMessage = "{\"method\": \"SUBSCRIBE\", \"params\": [\""+ symbol +"@depth@100ms\"], \"id\": 1}";
    }

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

bool Connection::performWebSocketHandshake() {
    std::string webSocketKey = "dGhlIHNhbXBsZSBub25jZQ==";
    std::ostringstream request;

    request << "GET /ws/"+ symbol +"@depth@100ms HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "Connection: Upgrade\r\n";
    request << "Upgrade: websocket\r\n";
    request << "Sec-WebSocket-Key: " << webSocketKey << "\r\n";
    request << "Sec-WebSocket-Version: 13\r\n";
    request << "\r\n";

    if (SSL_write(ssl, request.str().c_str(), static_cast<int>(request.str().length())) <= 0) {
        std::cerr << "Failed to send WebSocket handshake request." << std::endl;
        return false;
    }

    char buffer[4096];
    int bytesRead = SSL_read(ssl, buffer, sizeof(buffer));
    if (bytesRead <= 0) {
        std::cerr << "Failed to receive WebSocket handshake response." << std::endl;
        return false;
    }

    std::string response(buffer, bytesRead);
    if (response.find("Sec-WebSocket-Accept") == std::string::npos) {
        std::cerr << "Invalid WebSocket handshake response." << std::endl;
        return false;
    }
    return true;
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

    if (!performWebSocketHandshake()) {
        std::cerr << "WebSocket handshake failed.\n";
        return false;
    }

    return true;
}

void Connection::sendWebSocketMessage() {
    unsigned char* frame = new unsigned char[1 + subscribeMessage.size()];
    frame[0] = 0x81;
    std::copy(subscribeMessage.begin(), subscribeMessage.end(), frame + 1);
    if (SSL_write(ssl, frame, sizeof(frame)) <= 0) {
        std::cerr << "Failed to send WebSocket message." << std::endl;
    }
    delete[] frame;
}

void Connection::sendPongFrame(const std::string& payload) {
    std::vector<unsigned char> frame;
    frame.push_back(0xA);
    frame.push_back(static_cast<unsigned char>(payload.size()));
    frame.insert(frame.end(), payload.begin(), payload.end());
    SSL_write(ssl, frame.data(), static_cast<int>(frame.size()));
}

std::string Connection::receiveWebSocketMessage() {
    unsigned char buffer[4096];
    std::string completePayload;
    bool isPayloadComplete = false;
    std::size_t start;
    std::size_t back;

    while (!isPayloadComplete) {
        int bytesRead = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            int sslError = SSL_get_error(ssl, bytesRead);
            if (sslError != SSL_ERROR_WANT_READ && sslError != SSL_ERROR_WANT_WRITE) {
                std::cerr << "Failed to read WebSocket frame. SSL error code: " << sslError << std::endl;
                cleanup();
                #ifdef _WIN32
                WSACleanup();
                #endif
                ERR_print_errors_fp(stderr);
                std::exit(EXIT_FAILURE);
            }
            else continue;
        }
        unsigned char opcode = buffer[0] & 0x0F;
        if (bytesRead<15 && opcode == 0x9) {
            size_t payload_len = buffer[1] & 0x7F;
            SSL_write(ssl, buffer, static_cast<int>(payload_len));
            continue;
        }

        if (bytesRead<15 && (buffer[0] & 0x0F) == 0x9) {
            size_t payload_len = buffer[1] & 0x7F;
            size_t header_len = 4;
            std::string payload(reinterpret_cast<char*>(&buffer[header_len]), payload_len);
            sendPongFrame(payload);
            continue;
        }

        completePayload.append(reinterpret_cast<char*>(buffer), bytesRead);
        if(completePayload.find("{") == std::string::npos){
            completePayload.clear(); 
        }
        else{
            start = completePayload.find("{");
        }
        if (!completePayload.empty() && completePayload.find("}") != std::string::npos) {
            back = completePayload.find("}");
            isPayloadComplete = true;
        }
    }
    return completePayload.substr(start, back - start + 1);
}
