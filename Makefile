CC = gcc
CFLAGS = -Wall -g

test_forking: test_forking.o
	$(CC) $(CFLAGS) -o test_forking test_forking.o 
test_forking.o: test_forking.c
	$(CC) $(CFLAGS) -c test_forking.c 


