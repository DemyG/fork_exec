CC=gcc
CFLAGS=-I -Wall -g
OBJ = test_forking.c 
EXECUTABLE = test_forking
all: test_forking

$(EXECUTABLE): $(OBJ)
	    gcc -o $@ $^ $(CFLAGS)

clean:
	    rm -f $(EXECUTABLE)
