#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define N 22020096

int main(int argc, char* argv[]){
    int i, j;
    int *array;

    array = malloc(N*sizeof(int));
    for (j= 0; j< N; j++){
        for (i = 0; i < N; i++){
            array[i] = 0;
        }
    }

    for (j= 0; j< N; j++){
        for (i = 0; i < N; i++){
            array[i] = array[i] + 1;
        }
    }
    for (j=0; j<N; j++){
        for (i = 0; i < N; i++){
            array[i] = array[i] + 1;
        }
    }
    free(array);
    exit(1);
}
