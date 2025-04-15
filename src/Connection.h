#pragma once
#include <openssl/err.h>
#include <openssl/ssl.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
    typedef SOCKET socket_t;
    #define CLOSE_SOCKET closesocket
    #define SOCKET_INIT() WSAStartup(MAKEWORD(2, 2), &wsaData)
    #define SOCKET_CLEANUP() WSACleanup()
    #define INVALID_SOCKET_VAL INVALID_SOCKET
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    typedef int socket_t;
    #define CLOSE_SOCKET close
    #define SOCKET_INIT() 0
    #define SOCKET_CLEANUP() do {} while (0)
    #define INVALID_SOCKET_VAL -1
#endif

class Connection {
public:
    Connection(const std::string& host, const std::string& port, const std::string& symbol);
    ~Connection();
    bool setup();
    void cleanup();
    void sendWebSocketMessage();
    std::string receiveWebSocketMessage();

private:
    std::string host;
    std::string port;
    std::string symbol;
    std::string subscribeMessage;
    SSL_CTX* ctx;
    SSL* ssl;
    socket_t sockfd;
    bool performWebSocketHandshake();
    void sendPongFrame(const std::string& payload);
};
