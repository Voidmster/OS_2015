#ifndef PROXY_HTTP_SERVER_H
#define PROXY_HTTP_SERVER_H


#include "io_service.h"
#include <arpa/inet.h>


class local_server {
public:
    local_server(io_service &service, int port, std::function<void()> on_accept);
    ~local_server();
    posix_socket& get_socket();
    std::string get_local_endpoint();
private:
    io_service &service;
    posix_socket socket;
    io_event event;
    std::function<void()> on_accept;
    int port;
};


#endif //PROXY_HTTP_SERVER_H
