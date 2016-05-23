#include <signal.h>
#include <wait.h>
#include "rsh_daemon.h"

rsh_daemon::rsh_daemon(int port)
        : service(),
          server(service, port, std::bind(&rsh_daemon::create_new_left_side, this)),
          left_side_counter(0),
          right_side_counter(0)
{
    std::cerr << "!> Daemon server bind on " << server.get_local_endpoint()<< "\n";
}

posix_socket &rsh_daemon::get_server() {
    return server.get_socket();
}

io_service &rsh_daemon::get_service() {
    return service;
}

void rsh_daemon::create_new_left_side() {
    std::unique_ptr<left_side> u_ptr(new left_side(this, [this](left_side* item) {left_sides.erase(item);}));
    left_side *ptr = u_ptr.get();
    left_sides.emplace(ptr, std::move(u_ptr));
    if (++left_side_counter % 10 == 0) {
        std::cerr << "> " << left_side_counter  << " left_sides created\n";
    }

}

right_side *rsh_daemon::create_new_right_side(left_side *caller) {
    std::unique_ptr<right_side> u_ptr(new right_side(this, caller, [this](right_side* item) {right_sides.erase(item);}));
    right_side *ptr = u_ptr.get();
    right_sides.emplace(ptr, std::move(u_ptr));
    if (++right_side_counter % 10 == 0) {
        std::cerr << "> " << right_side_counter  << " right_sides created\n";
    }
    return ptr;
}

void rsh_daemon::run() {
    service.run();
}

left_side::left_side(rsh_daemon *parent, std::function<void(left_side*)> on_disconnect)
        : parent(parent),
          socket(parent->get_server().accept()),
          partner(nullptr),
          ioEvent(parent->get_service(), socket.get_fd(), EPOLLRDHUP, [this] (uint32_t events) mutable throw(std::runtime_error)
          {
              try {
                  if (events & EPOLLIN) {
                      read_request();
                  }
                  if (events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                      this->on_disconnect(this);
                  }
                  if (events & EPOLLOUT) {
                      send_response();
                  }
              } catch (std::exception &e) {
                  this->on_disconnect(this);
              }

          }),
          on_disconnect(on_disconnect)
{
    partner = parent->create_new_right_side(this);
    std::cerr << "Left_side created" << "\n";
}

left_side::~left_side() {
    if (partner) {
        partner->partner = nullptr;
        partner->on_disconnect(partner);
    }
    std::cerr << "Left_side destroyed" << "\n";
}

void left_side::read_request() {
    std::cerr << "in read_request" << "\n";
    if (partner) {
        if (partner->buffer.can_read_into()) {
            partner->buffer.read_from(socket.get_fd());

            if (!partner->buffer.can_read_into()) {
                ioEvent.remove_flag(EPOLLIN);
            }
            if (partner->buffer.can_write_from()) {
                partner->ioEvent.add_flag(EPOLLOUT);
            }
            if (buffer.can_write_from()) {
                ioEvent.add_flag(EPOLLOUT);
            }
        }
    }
}

void left_side::send_response() {
    if (partner) {
        if (buffer.can_write_from()) {
            buffer.write_to_fd(socket.get_fd());

            if (!buffer.can_write_from()) {
                ioEvent.remove_flag(EPOLLOUT);
            }

            if (buffer.can_read_into()) {
                partner->ioEvent.add_flag(EPOLLIN);
            }

            if (partner->buffer.can_read_into()) {
                ioEvent.add_flag(EPOLLIN);
            }
        }
    }
}

right_side::right_side(rsh_daemon *parent, left_side *partner, std::function<void(right_side *)> on_disconnect)
        : partner(partner),
          ps_term(create_ps_term()),
          ioEvent(parent->get_service(), ps_term, EPOLLIN, [this] (uint32_t events) mutable throw(std::runtime_error)
          {
              try {
                  if (events & EPOLLIN) {
                      read_response();
                  }
                  if (events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                      this->on_disconnect(this);
                      return;
                  }
                  if (events & EPOLLOUT) {
                      send_request();
                  }
              } catch (std::exception &e) {
                  this->on_disconnect(this);
              }
          }),
          on_disconnect(on_disconnect),
          parent(parent)
{
    std::cerr << "Right_side created" << "\n";
}

right_side::~right_side() {
    if (partner){
        partner->partner = nullptr;
        partner->on_disconnect(partner);
    }

    kill(child, SIGKILL);
    int status;
    waitpid(child, &status, 0);
    std::cerr << "Right_side destroyed" << "\n";
}


void right_side::send_request() {
    if (partner) {
        if (buffer.can_write_from()) {
            buffer.write_to_fd(ps_term);

            if (!buffer.can_write_from()) {
                ioEvent.remove_flag(EPOLLOUT);
            }

            if (buffer.can_read_into()) {
                partner->ioEvent.add_flag(EPOLLIN);
            }

            if (partner->buffer.can_read_into()) {
                ioEvent.add_flag(EPOLLIN);
            }
        }
    }
}

void right_side::read_response() {
    if (partner) {
        if (partner->buffer.can_read_into()) {
            partner->buffer.read_from(ps_term);

            if (!partner->buffer.can_read_into()) {
                ioEvent.remove_flag(EPOLLIN);
            }
            if (partner->buffer.can_write_from()) {
                partner->ioEvent.add_flag(EPOLLOUT);
            }
            if (buffer.can_write_from()) {
                ioEvent.add_flag(EPOLLOUT);
            }
        }
    }
}

int right_side::create_ps_term() {
    int master;
    if ((master = posix_openpt(O_RDWR)) == -1) { throw_error("posix_openpt()"); }
    if (grantpt(master) == -1) { throw_error("grantpt()"); }
    if (unlockpt(master) == -1) { throw_error("inlockpt()"); }

    int slave = open(ptsname(master), O_RDWR);
    if (slave == -1) { throw_error("open(ptsname(master)..."); }

    child = fork();
    if (child == -1) { throw_error("fork()"); }

    if (!child) {
        struct termios settings;
        tcgetattr(slave, &settings);
        cfmakeraw(&settings);
        tcsetattr(slave, TCSANOW, &settings);

        close(master);
        if (dup2(slave, STDIN_FILENO) == -1) {throw_error("dup2(slave, STDIN_FILENO)");}
        if (dup2(slave, STDOUT_FILENO) == -1) {throw_error("dup2(slave, STDOUT_FILENO)");}
        if (dup2(slave, STDERR_FILENO) == -1) {throw_error("dup2(slave, STDERR_FILENO)");}
        close(slave);

        setsid();
        ioctl(0, TIOCSCTTY, 1);

        if (execlp("/bin/sh", "sh", NULL) == -1) {throw_error("execlp()");}
    } else {
        close(slave);
    }

    return master;
}