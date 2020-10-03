CC = g++
CFLAGS = -c -Wall -O2 -std=c++11
PROGRAM = shell379
OBJS = shell379.o command_executer.o process_table.o

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) -o $(PROGRAM)

shell379.o: shell379.cpp
	$(CC) $(CFLAGS) shell379.cpp -o shell379.o
	
command_executer.o: command_executer.h command_executer.cpp
	$(CC) $(CFLAGS) command_executer.cpp -o command_executer.o 

process_table.o: process_table.h process_table.cpp
	$(CC) $(CFLAGS) process_table.cpp -o process_table.o

clean:
	@rm -f $(OBJS)
	@rm -f $(PROGRAM)