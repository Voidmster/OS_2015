//
// Created by daniil on 23.05.16.
//

#include "data_buffer.h"

data_buffer::data_buffer() : read_ind(0), write_ind(0) {}

bool data_buffer::can_read_into() {
    return read_ind < MAX_SIZE;
}

bool data_buffer::can_write_from() {
    return write_ind < MAX_SIZE;
}

void data_buffer::read_from(file_descriptor &fd) {
    read_ind += fd.read_some(buffer + read_ind, MAX_SIZE - read_ind);
}

void data_buffer::write_to_fd(file_descriptor &fd) {
    write_ind += fd.write_some(buffer + write_ind, read_ind - write_ind);
    if (write_ind == MAX_SIZE) {
        write_ind = read_ind = 0;
    }
}



