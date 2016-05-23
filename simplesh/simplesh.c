#include <iostream>
#include <vector>
#include <algorithm>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

const int BUFFER_SIZE = 1024;

std::string rest = "";
std::vector<pid_t> pids;

void custom_handler(int sign, siginfo_t* siginfo, void* context) {
  for (int i = 0; i < pids.size(); i++) {
    kill(pids[i], sign);
  }
}

std::string read_line(int fd) {
  std::vector<char> buffer(BUFFER_SIZE, 0);
  std::string result(rest);
  int _size, total = 0;
  rest = "";
  bool flag = false;

  while(true) {
    _size = read(fd, buffer.data(), buffer.size());

    if (_size < 0) {
      if (errno == EINTR) {
        flag = true;
        break;
      }
      exit(1);
    }

    if (_size == 0) {
      break;
    }

    total += _size;
    std::vector<char>::iterator index = std::find(buffer.begin(), buffer.begin() + _size, '\n');

    if (index == buffer.begin() + _size) {
      result.append(buffer.data());
    } else {

      std::vector<char> main_part(buffer.begin(), index++);
      std::vector<char> rest_part(index, buffer.begin() + _size);

      result.append(main_part.data(), main_part.size());
      rest.append(rest_part.data(), rest_part.size());
      break;
    }
  }

  if (total == 0 && !flag) {
    exit(0);
  }
  return result;
}

void run(std::string line) {
  line.append(" ");
  std::string::iterator begin = line.begin();
  std::string::iterator end = line.end();
  int argc = 0;
  int pipel[2];
  int piper[2];

  while (begin != end) {
    std::vector<char*> args;
    std::string::iterator index;

    for (index = begin; index != end; index++) {
        if (*index == ' ') {
          if (index != begin) {
            std::string current(begin, index);
            args.push_back(strdup(current.c_str()));
          }
          begin = index + 1;
        } else if (*index == '|') {
          if (begin != index){
            std::string current(begin, index);
            args.push_back(strdup(current.c_str()));
          }
          begin = index + 1;
          break;
        }
    }
    args.push_back(NULL);

    pipe(piper);

    pid_t pid = fork();
    if (pid == 0) {
      if (argc) {
        dup2(pipel[0], 0);
        close(pipel[0]);
        close(pipel[1]);
      }

      if (index != end) {
        dup2(piper[1], 1);
      }

      close(piper[0]);
      close(piper[1]);

      if (execvp(args[0], args.data()) == -1) {
          perror("Error while execvp");
          exit(1);
      }
    } else {
      if (argc) {
        close(pipel[0]);
        close(pipel[1]);
      }

      if (index == end) {
        close(piper[0]);
        close(piper[1]);
      }

      pids.push_back(pid);

      int status;
      waitpid(pid, &status, 0);
    }

    pipel[0] = piper[0];
    pipel[1] = piper[1];
    argc++;
  }
}

int main() {
  struct sigaction act;
  act.sa_handler = (__sighandler_t) custom_handler;
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGINT, &act, NULL);

  while (true) {
      if (write(STDOUT_FILENO, "$ ", 2) == -1) {
        std::cerr << "Error in initial write\n";
        exit(1);
      }

      std::string line = read_line(STDIN_FILENO);
      if (line == "") {
        continue;
      } else {
        run(line);
      }
  }
  return 0;
}
