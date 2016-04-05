#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>


using namespace std;

const int BUFFER_SIZE = 2048;

vector < vector < vector < char> > > all_progs;
vector <vector <char> > program;
vector <char> line;

int write_buffer(int fd, void *buf, size_t buf_size) {
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

int read_buffer(int fd, void *buf, size_t buf_size) {
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

void dump() {
    for (int i = 0; i < all_progs.size(); i++) {
        for (int j = 0; j < all_progs[i].size(); j++) {
            for (int k = 0; k < all_progs[i][j].size(); k++) {
                cout << all_progs[i][j][k];
            }
            cout << endl;
        }
        cout << endl;
    }
}

void parse_program() {
    char *name;
    char **args;
    vector <char> temp;

    for (int i = 0; i < line.size(); i++) {
        if (line[i] == '|') {
            if (temp.size() != 0) {
                program.push_back(temp);
                temp.clear();
            }
            all_progs.push_back(program);
            program.clear();
        } else {
            if (line[i] == ' ') {
                if (temp.size() != 0) {
                    program.push_back(temp);
                    temp.clear();
                }
            } else {
                temp.push_back(line[i]);
            }
        }
    }

    char *probe;
    if (temp.size() != 0) {
        program.push_back(temp);
        temp.clear();
    }
    all_progs.push_back(program);
    program.clear();
}

void add_to_line(char *buf, size_t _size) {
    for (int i = 0; i < _size; i++) {
        line.push_back(buf[i]);
    }
}

int read_line() {
    char buf[BUFFER_SIZE];
    int count = 0;
    int buf_size = BUFFER_SIZE;

    while (1) {
        int res = read(STDIN_FILENO, buf, BUFFER_SIZE);
        if (res < 0) return -1;
        if (res == 0) break;

        if (*(buf + res - 1) == '\n') {
            add_to_line(buf, count + res - 1);
            parse_program();
            break;
        } else {
            add_to_line(buf, count + res);
        }
    }

    return 1;
}

void custom_copy(int f, int s, char *buf) {
    for (int i = 0; i < all_progs[f][s].size(); i++) {
        buf[i] = all_progs[f][s][i];
    }

    buf[all_progs[f][s].size()] = '\0';
}

void run_progs() {
    char **args;
    vector<char> empt;

    for (int i = 0; i < all_progs.size(); i++) {
        args = new char*[all_progs[i].size() + 1];
        for (int j = 0; j < all_progs[i].size(); j++) {
            args[j] = new char[all_progs[i][j].size()];
            copy(all_progs[i][j].begin(), all_progs[i][j].end(), args[j]);
        }
        args[all_progs[i].size()] = (char) NULL;

        execvp(args[0], args);
    }
}

int main() {
    char buf[BUFFER_SIZE];
    char *hello = "$ ";

    while (1) {
        write_buffer(STDIN_FILENO, hello, 2);

        int res = read_line();
        if (res < 0) return -1;
        run_progs();
        break;

        res = write_buffer(STDOUT_FILENO, buf, res);
        if (res < 0) return -1;
        break;
    }
    return 0;
}
