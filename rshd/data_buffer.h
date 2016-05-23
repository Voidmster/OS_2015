//
// Created by daniil on 23.05.16.
//

#ifndef RSHD_DATA_BUFFER_H
#define RSHD_DATA_BUFFER_H


#include <string>
#include "file_descriptor.h"

class data_buffer {
    const static size_t MAX_SIZE = 4096;
public:
    data_buffer();

    bool can_read_into();
    bool can_write_from();

    void read_from(file_descriptor &fd);
    void write_to_fd(file_descriptor &fd);
private:
    size_t read_ind;
    size_t write_ind;
    char buffer[MAX_SIZE];
};

#endif //RSHD_DATA_BUFFER_H
