#include <iostream>
#include "local_server.h"


local_server::local_server(io_service &service, int port, std::function<void()> on_accept)
        : service(service),
          socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK),
          event(service, socket.get_fd(), EPOLLIN, [this](uint32_t event)
          {
              if (event == EPOLLIN) {
                this->on_accept();
              }
          }),
          on_accept(on_accept),
          port(port)
{
    socket.bind(AF_INET, htons(port), INADDR_ANY);
    socket.listen();
    std::cerr << "Server socket created\n";
}

posix_socket &local_server::get_socket() {
    return socket;
}

std::string local_server::get_local_endpoint() {
    return "localhost " + std::to_string(port);
}

local_server::~local_server() {
    std::cerr << "server destroyed\n";
}











