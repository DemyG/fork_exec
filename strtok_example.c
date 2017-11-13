#include <string.h>
#include <stdio.h>
#include <stdlib.h>



char *paramList[12];

int main () {
		FILE *f = fopen("specs.txt", "r");
		char * line = NULL;
		size_t len = 0;
		ssize_t read;
		int i =0;
		if (f == NULL){
				printf("failed to open the file\n");
				exit(2);
		}

		while ((read = getline(&line, &len, f)) != -1) {
				//printf("Retrieved line of length %zu :\n", read);
				//printf("%s", line);


				//char str[] = "./soplex_base.gcc49 -s1 -e -m45000 pds-50.mps";
				const char* s = " ";
				char *token;
				paramList[0] = "taskset";
				paramList[1] = "-c";
				paramList[2] = "0";
				/* get the first token */
				token = strtok(line, s);
				int i = 3;
				/* walk through other tokens */
				while( token != NULL ) {
						paramList[i] = strdup(token);
						token = strtok(NULL, s);
						i++;
				}
				paramList[i-1][strlen(paramList[i-1]) - 1] = '\0';
				paramList[i] = NULL;
				i = 0;
				while(paramList[i] != NULL){
						printf( "%s ", paramList[i]);
						i++;
				}
				printf( "\n");
		}
		free(line);
		fclose(f);
		printf("New output\n");
		i = 0;
		while(paramList[i] != NULL){
				printf( "%s ", paramList[i]);
                if (i>=3)
                    free(paramList[i]);
				i++;
		}
				printf( "\n");
		return(0);
}
