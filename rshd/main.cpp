#include <iostream>
#include "rsh_daemon.h"

using namespace std;

void daemonize() {
    pid_t pid = fork();
    if (pid == 0) {
        setsid();

        pid_t parent = fork();

        if (parent == 0) {
            int file = open("/tmp/rshd.pid", O_RDWR | O_CREAT | O_TRUNC, 0664);
            if (file == -1) {
                printf("Failed to create \"/tmp/rshd.pid\"\n");
                exit(0);
            }
            pid = getpid();
            std::string pid_string = std::to_string(pid);
            write(file, pid_string.c_str(), pid_string.size());
            close(file);

            chdir("/");
            int err_out = open("tmp/rshd.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (err_out == -1) {
                printf("Failed to create \"/tmp/rshd.pid\"\n");
                exit(0);
            }
            dup2(err_out, STDERR_FILENO);
            close(err_out);
            return;
        } else {
            exit(0);
        }
    } else {
        exit(0);
    }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cerr << "bad format";
        return 1;
    }
    daemonize();
    rsh_daemon daemon(std::atoi(argv[1]));
    daemon.run();

    return 0;
}