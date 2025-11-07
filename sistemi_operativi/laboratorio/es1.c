#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>


void handler(int signum) {
    printf("Signal %d received\n", signum);
    sleep(2);
}

int main() {
    signal(SIGINT, handler);
    while(1) {
        printf("Executing...\n");
    }
    return 0;
}
