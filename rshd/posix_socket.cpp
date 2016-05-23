#include <iostream>
#include "posix_socket.h"

posix_socket::posix_socket(int domain, int type) : fd(create_socket_fd(domain, type)) {}

posix_socket::~posix_socket() { }

int posix_socket::get_flags() {
    int result = fcntl(fd.get_fd(), F_GETFD);
    if (result == -1) {
        throw_error("Error in get_flags");
    }
    return result;
}

void posix_socket::set_flags(int nex_flags) {
    if (fcntl(fd.get_fd(), F_GETFD, nex_flags) == -1) {
        throw_error("Error in get_flags");
    }
}

void posix_socket::bind(sa_family_t sa_family, uint16_t port, in_addr_t s_addr) {
    struct sockaddr_in hints;

    hints.sin_family = sa_family;
    hints.sin_port = port;
    hints.sin_addr.s_addr = s_addr;

    if (::bind(fd.get_fd(), (struct sockaddr*) &hints, sizeof(hints)) == -1) {
        throw_error("Error in bind posix_socket");
    }
}

void posix_socket::listen() {
    if (::listen(fd.get_fd(), SOMAXCONN) == -1) {
        throw_error("Error in listen posix_socket");
    }
}

int posix_socket::accept() {
    int result = ::accept(fd.get_fd(), NULL, NULL);
    if (result == -1) {
        throw_error("Error in accept posix_socket");
    }

    return result;
}

int posix_socket::get_available_bytes() {
    return fd.get_available_bytes();
}

ssize_t posix_socket::read_some(void *buffer, size_t size) {
    return fd.read_some(buffer, size);
}

ssize_t posix_socket::write_some(const char *buffer, size_t size) {
    return fd.write_some(buffer, size);
}


file_descriptor& posix_socket::get_fd() {
    return fd;
}

posix_socket::posix_socket() : fd(-1) {}

posix_socket::posix_socket(int accepted_fd) : fd(accepted_fd) {
    set_flags(get_flags() | SOCK_STREAM | SOCK_NONBLOCK);
}

void posix_socket::connect(sockaddr *adr, socklen_t addrlen) {
    if ((::connect(fd.get_fd(), adr, addrlen)) == -1) {
        if (errno != EINPROGRESS) {
            throw_error("Error in posix_socket::connect");
        }
    }
}

void posix_socket::read_input(std::string &s) {
    fd.read_input(s);
}

int posix_socket::create_socket_fd(int domain, int type) {
    int r;
    if ((r = socket(domain, type, 0)) == -1) {
        throw_error("Can't create posix_socket");
    }
    return r;
}









































