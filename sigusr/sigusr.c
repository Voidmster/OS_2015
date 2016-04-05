#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void custom_handler(int sign, siginfo_t* siginfo, void* context){
    if (sign == SIGUSR1) {
        printf("SIGUSR1 from %i\n", siginfo->si_pid);
    } else if (sign == SIGUSR2) {
        printf("SIGUSR2 from %i\n", siginfo->si_pid);
    }
    exit(0);
}

int main()
{
    struct sigaction act1;
    act1.sa_handler = (__sighandler_t) custom_handler;
    act1.sa_flags = SA_SIGINFO;
    sigaddset(&act1.sa_mask, SIGUSR2);

    struct sigaction act2;
    act2.sa_handler = (__sighandler_t) custom_handler;
    act2.sa_flags = SA_SIGINFO;
    sigaddset(&act2.sa_mask, SIGUSR1);

    sigaction(SIGUSR1, &act1, NULL);
    sigaction(SIGUSR2, &act2, NULL);

    sleep(10);

    printf("No signals were caught\n");

    return 0;
}
