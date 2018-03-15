CC=gcc
CFLAGS=-Wall -g
OBJ =monitor.c 
EXECUTABLE = test_forking
all: test_forking

$(EXECUTABLE): $(OBJ)
	    gcc -o $@ $^ $(CFLAGS)

clean:
	    rm -f $(EXECUTABLE)
