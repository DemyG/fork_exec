#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char* argv[]){
    int time_to_sleep = atoi(argv[1]);
    printf("sleeping for %d seconds\n)", (time_to_sleep+1)*10);
    sleep((time_to_sleep+1)*10);
    exit(1);
}
