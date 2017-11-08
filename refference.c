#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NUM 2


int deploy_an_instance(pid_t *ptr_pid, int offset){
    pid_t pid;
    if ((pid = fork()) < 0)
        perror("fork() error");
    else if (pid == 0) {
        sleep(10*(offset+1));
        /*char *index = &offset;
        char *const paramList[] = {"./child_program", index, NULL};
        execv("./child_program", paramList);*/
        exit(1);
    }
    else{
        ptr_pid[offset] = pid;
        printf("child%d pid %d\n",offset, pid);
    }

    return pid;

}

int main() {
    pid_t pid_table[NUM], pid;
    time_t t;
    int status, i, stopped_processes = 0, status_table[NUM];

    for (i=0; i<NUM; i++){
        // deploy an instace
        pid = deploy_an_instance(pid_table, i);
        status_table[i] = 0;
    }

    if (pid > 0){
        do {
            for(i =0; i<NUM; i++){
                if ((pid = waitpid(pid_table[i], &status, WNOHANG)) == -1){
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
                        deploy_an_instance(pid_table, i);
                        printf("child with pid %d forked\n", pid_table[i]);
                    }
                    else printf("child with pid %d did not exit successfully", pid_table[i]);
                }
            }
        } while (stopped_processes < NUM);
    }

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
