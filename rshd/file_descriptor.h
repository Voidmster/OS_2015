#ifndef PROXY_SERVER_FILE_DESCRIPTOR_H
#define PROXY_SERVER_FILE_DESCRIPTOR_H

#include <unistd.h>
#include <errno.h>
#include "utils.h"
#include <sys/ioctl.h>

class file_descriptor {
public:
    file_descriptor();
    file_descriptor(int fd);
    ~file_descriptor();

    int get_available_bytes();
    void read_input(std::string &s);
    ssize_t read_some(void *buffer, size_t size);
    ssize_t write_some(const char *buffer, size_t size);

    void close();
    int& get_fd();
private:
    int fd;
};


#endif //PROXY_SERVER_FILE_DESCRIPTOR_H
