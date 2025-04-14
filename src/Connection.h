#pragma once
#include <string>
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
    Connection(const std::string& host, const std::string& port = "443");
    ~Connection();
    bool setup();
    std::string getPayload(const std::string& path);
    void cleanup();
private:
    std::string host;
    std::string port;
    SSL_CTX* ctx;
    SSL* ssl;
    socket_t sockfd;
    struct hostent* server;
};
