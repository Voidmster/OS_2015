#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

const int BUFFER_SIZE = 2048;

int _read(int fd, void *buf, size_t buf_size) {
    int count = 0;
    while (buf_size) {
        int res = read(fd, buf, buf_size);
        if (res == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        }
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
        if (res == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        }
        if (res == 0) break;

        buf_size -= res;
        count += res;
        buf += res;
    }

    return count;
}

int cat(int fd) {
    char buf[BUFFER_SIZE];
    while (1) {
        int res = _read(fd, buf, BUFFER_SIZE);
        if (res < 0) return -1;
        if (res == 0) break;
        res = _write(STDOUT_FILENO, buf, res);
        if (res < 0) return -1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        int i;
        for (i = 1; i < argc; i++) {
            if (cat(open(argv[1], O_RDONLY)) == -1) {
                return -1;
            }
        }
    } else {
        return cat(STDIN_FILENO);
    }

    return 0;
}

