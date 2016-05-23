#ifndef PROXY_PROXY_H
#define PROXY_PROXY_H


#include <vector>
#include <memory>
#include <map>
#include <set>
#include <queue>
#include "local_server.h"
#include "io_service.h"
#include "data_buffer.h"
#include <termios.h>
#include <iostream>


class left_side;
class right_side;

class rsh_daemon {
    friend class right_side;
public:
    rsh_daemon(int port);
    posix_socket& get_server();
    io_service& get_service();

    void create_new_left_side();
    void run();

    right_side* create_new_right_side(left_side*);
private:
    io_service service;
    local_server server;
    std::map<left_side*, std::unique_ptr <left_side> > left_sides;
    std::map<right_side*, std::unique_ptr <right_side> > right_sides;

    int left_side_counter;
    int right_side_counter;
};

class left_side {
    friend class right_side;
public:
    left_side(rsh_daemon *parent, std::function<void(left_side*)> on_disconnect);
    ~left_side();

    void read_request();
    void send_response();
private:
    rsh_daemon* parent;
    posix_socket socket;
    io_event ioEvent;
    right_side* partner;
    data_buffer buffer;
    std::function<void(left_side*)> on_disconnect;
};

class right_side {
    friend class left_side;
public:
    right_side(rsh_daemon *parent, left_side* partner, std::function<void(right_side*)> on_disconnect);
    ~right_side();

    void send_request();
    void read_response();
private:
    rsh_daemon* parent;
    file_descriptor ps_term;
    io_event ioEvent;
    left_side* partner;
    data_buffer buffer;
    std::function<void(right_side*)> on_disconnect;
    int create_ps_term();
    pid_t child;
};

#endif //PROXY_PROXY_H
