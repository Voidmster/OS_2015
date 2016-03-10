#include <stdio.h>
#include <unistd.h>

const int BUFFER_SIZE = 2048;

int _read(int fd, void *buf, size_t buf_size) {
    int count = 0;
    while (buf_size) {
        int res = read(fd, buf, buf_size);
        if (res < 0) return -1;
        if (res == 0) break;

        buf_size -= res;
        count += res;
        buf += res;
    }

    return count;
}

int _write(int fd, void *buf, size_t buf_size) {
    int count = 0;
    while (buf_size) {
        int res = write(fd, buf, buf_size);
        if (res < 0) return -1;
        if (res == 0) break;

        buf_size -= res;
        count += res;
        buf += res;
    }

    return count;
}

int main() {
    char buf[BUFFER_SIZE];
    while (1) {
        int res = _read(STDIN_FILENO, buf, BUFFER_SIZE);
        if (res < 0) return -1;
        if (res == 0) break;
        res = _write(STDOUT_FILENO, buf, res);
        if (res < 0) return -1;
    }
    return 0;
}
