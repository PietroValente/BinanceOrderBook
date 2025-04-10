#pragma once
#include <string>
#include <openssl/err.h>
#include <openssl/ssl.h>

#pragma comment(lib, "ws2_32.lib")

class Connection {
public:
    Connection(const std::string& host, const std::string& port = "443");
    ~Connection();
    bool setup();
    std::string getJson(const std::string& path);
    void cleanup();
private:
    std::string host;
    std::string port;
    SSL_CTX* ctx;
    SSL* ssl;
    SOCKET sockfd;
    struct hostent* server;
};
