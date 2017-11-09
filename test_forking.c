#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include<signal.h>

#define NUM 1

static pid_t pid_table[NUM];
static int STOP = 0;
static char *current_core;
static void CtrlC_sighandler(int signo){
		int i;
		STOP = 1;
		for (i =0; i< NUM; i++){
				printf("signal handler killed child%d with pid %d\n", i, pid_table[i]);
				kill(pid_table[i], SIGKILL);

		}


}

int deploy_low_prio_instance(pid_t *ptr_pid, int offset){
		pid_t pid;
		if ((pid = fork()) < 0)
				perror("fork() error");
		else if (pid == 0) {
                //char *buf = "2";
                //char buf;
                printf("pid_table[NUM], %d\n", pid_table[0]);
				//sleep(10*(offset+1));
                printf("current core: %s\n", current_core);
				//snprintf(buf, sizeof(int), "%d", current_core);
                //buf = (char)current_core;
                //printf("%d\n", buf);
                char *const paramList[] = {"/usr/bin/taskset", "-c", current_core, "./low_prio_program", NULL};
				execv("/usr/bin/taskset", paramList);
				//exit(1);
		}
		else{
				ptr_pid[offset] = pid;
				printf("child%d pid %d\n",offset, pid);
		}

		return pid;

}


int deploy_high_prio_instance(pid_t *ptr_pid, int offset, char* executable_name){
		pid_t pid;
		if ((pid = fork()) < 0)
				perror("fork() error");
		else if (pid == 0) {
				//sleep(10*(offset+1));
				char *const paramList[] = {executable_name, NULL};
				execv("./high_prio_program", paramList);
				//exit(1);
		}
		else{
				ptr_pid[offset] = pid;
				printf("child%d pid %d\n",offset, pid);
		}

		return pid;

}

int main() {
		pid_t pid;
		time_t t;
		int status, i, stopped_processes = 0, status_table[NUM];

		if(signal(SIGINT, CtrlC_sighandler) == SIG_ERR)
				printf("\ncan't catch SIGINT\n");

        char core[3] = {"0", "1", "2"};
		char *executable_name = "./high_prio_program";
		for (i=0; i<NUM; i++){
				// deploy an instace
                printf("core:%s\n", core[i]);
                current_core = core[i];
				if (i != 1)
						pid = deploy_low_prio_instance(pid_table, i);
				else
						pid = deploy_high_prio_instance(pid_table, i, executable_name);

				status_table[i] = 0;
		}

		if (pid > 0){
				do {
						for(i =0; i<NUM; i++){
								if ((pid = waitpid(pid_table[i], &status, WNOHANG)) == -1){
										sleep(1);
										perror("wait() error");
								}
								else if (pid == 0) {
										time(&t);
										printf("child with pid %d is still running at %s", pid_table[i], ctime(&t));
										sleep(1);
								}
								else {
										if (WIFEXITED(status)){
												printf("child with pid %d exited with status of %d\n", pid_table[i], WEXITSTATUS(status));
												if (status_table[i] == 0){
														stopped_processes++;
														status_table[i] = 1;
												}
												// restart an instace
												if (i != 0)
														pid = deploy_low_prio_instance(pid_table, i);
												else
														pid = deploy_high_prio_instance(pid_table, i, executable_name);
												printf("child with pid %d forked\n", pid_table[i]);
										}
										else printf("child with pid %d did not exit successfully", pid_table[i]);
								}
						}
				} while (stopped_processes < NUM && (!STOP));
		}

		if (!STOP){
				for (i=0; i<NUM; i++){
						if ((waitpid(pid_table[i], &status, WNOHANG)) == 0){
								printf("father killed child%d with pid: %d\n", i, pid_table[i]);
								kill(pid_table[i], SIGKILL);
						}
						else{
								if (WIFEXITED(status))
										printf("=>child%d exited with status of %d\n", i, WEXITSTATUS(status));


						}
				}
		}
		else
				printf("Everything is dead because of a signal\n");
}
