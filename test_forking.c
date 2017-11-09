#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define NUM 2

static pid_t pid_table[NUM];
static int STOP = 0;
static char *current_core;

static struct process_info{
    char *executable;
    char *core;
    int  status;
};

struct process_info *info_table[NUM];

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
        printf("pid_table[NUM], %d\n", pid_table[0]);
        printf("current core: %s\n", current_core);
        char *const paramList[] = {"/usr/bin/taskset", "-c", "1", "./low_prio_program", NULL};
        execv("/usr/bin/taskset", paramList);
        //exit(1);
    }
    else{
        ptr_pid[offset] = pid;
        printf("child%d pid %d\n",offset, pid);
    }

    return pid;

}


int deploy_high_prio_instance(pid_t *ptr_pid, int offset, char* core, char* executable_name){
    pid_t pid;
    if ((pid = fork()) < 0)
        perror("fork() error");
    else if (pid == 0) {
        char *const paramList[] = {"/usr/bin/taskset", "-c", core, executable_name, NULL};
        execv("/usr/bin/taskset", paramList);
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


    char *executable_name_1 = "./high_prio_program";
    char *executable_name_2 = "./low_prio_program";
    for (i=0; i<NUM; i++){
        info_table[i] =  malloc(sizeof(struct process_info));
        if (i == 0){
            info_table[i]->executable = executable_name_1;
            info_table[i]->core = "0";
        }
        else{
            switch(i){
            info_table[i]->executable = executable_name_2;
                case 1:
                    info_table[i]->core = "1";
                        break;
                case 2:
                    info_table[i]->core = "2";
                        break;
            }
        }
        info_table[i]->status = 0;
    }

    for (i=0; i<NUM; i++){
        // deploy an instace
        if (i != 0)
            pid = deploy_low_prio_instance(pid_table, i);
        else
            pid = deploy_high_prio_instance(pid_table, i, info_table[i]->core, info_table[i]->executable);

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
                            pid = deploy_high_prio_instance(pid_table, i, info_table[i]->core, info_table[i]->executable);
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
