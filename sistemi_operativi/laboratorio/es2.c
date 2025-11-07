#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define NCHILDREN 3

void handler(int sig) {
	printf("Sono il processo %d e ho ricevuto il segnale %d\n", getpid(), sig);
	exit(1);
}

int main()
{
	pid_t pid;
	int children = 0;

	signal(SIGUSR1, handler); // SIGUSR1 is handled by handler

	// creation loop
	while (1) {
		children++; // number of children created
		pid = fork();
		if (pid == 0) {
			// child process
			printf("Sono il figlio %d\n", getpid());

			// all children but the first enter this infinite loop
			if (children > 1)
				while(1);

			//  children exit
			exit(0);
		}
		else if (pid > 0) {
			// parent process
			// generate up to NCHILDREN processes
			if (children < NCHILDREN)
				continue;

			sleep(3); // sleep for 3 seconds
			signal(SIGUSR1, SIG_IGN); // ignore signal SIGUSR1
			kill(0, SIGUSR1); // send SIGUSR1 to my process group

		    // wait for all the children
		    while (1) {
				int status;
				pid_t term_child = wait(&status); // returns pid of the terminated child
				if (term_child < 0)
					break;	// no more children left
			    if (WIFEXITED(status)) {
				    printf("Terminazione di %d volontaria\n", term_child);
				    printf("Stato di terminazione: %d\n", WEXITSTATUS(status));
				} else
					printf("Terminazione di %d involontaria\n", term_child);
			}
			exit(0);
		}
		else {
			printf("Creazione fallita!\n");
			exit(1);
		}
	}
}
