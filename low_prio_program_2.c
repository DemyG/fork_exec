#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define N 10

int main(int argc, char* argv[]){
    int i;
	int *array;
	
	array = malloc(sizeof(N)*sizeof(int));
	for (i = 0; i < N-1; i++){
		printf("sizeof(array) %d\n",sizeof(*array));
		array[i] = 1;
	}
	
	free(array);
	exit(1);
}
