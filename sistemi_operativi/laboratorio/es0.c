#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
void handler(int signum) {
    printf("Signal %d received\n", signum);
}

int main() {
    signal(SIGALRM, handler);
    alarm(1);
    int p = pause();
    printf("Pause returned %d\n", p);
    return 0;
}
