#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

volatile int flag = 0;

void custom_handler(int sign, siginfo_t* siginfo, void* context) {
    if (flag == 0) {
      printf("Signal #%d from %d\n", sign, siginfo->si_pid);
    } else {
      printf("No signals were caught\n");
    }

    exit(0);
}

int main()
{
    struct sigaction act;
    act.sa_handler = (__sighandler_t) custom_handler;
    act.sa_flags = SA_SIGINFO;
    sigfillset(&act.sa_mask);

    int i = 1;
    while (i != 32) {
      if (i != SIGSTOP && i != SIGKILL) {
        if (sigaction(i, &act, NULL) == -1) {
          printf("Can't set sig_handler for #%d\n", i);
          exit(0);
        }
      }
      i = i + 1;
    }
    sleep(10);
    flag = 1;
    raise(SIGUSR1);

    return 0;
}
