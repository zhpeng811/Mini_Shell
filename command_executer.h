/** 
 * CMPUT 379 Fall 2020 Assignment 1
 * Name: Ze Hui Peng
 * ccid: zhpeng
 * id: 1594884
 * 
 * command_executer.h
 */

#ifndef _COMMAND_EXECUTER_H
#define _COMMAND_EXECUTER_H

#include <errno.h> 
#include <fcntl.h> // open() 
#include <iostream> // cin, cout
#include <signal.h> // kill()
#include <stdio.h> // popen(), pclose(), perror()
#include <sys/resource.h> //gerrusage()
#include <sys/wait.h> // wait(), waitpid(), fork(), execvp()
#include <unistd.h> // dup(), dup2(), _exit()


#include "process_table.h"

using namespace std;

// Assignment specified constants
#define LINE_LENGTH 100 // Max number of characters for input line
#define MAX_ARGS 7 // max number of arguments to a command
#define MAX_LENGTH 20 // max number of characters in an argument
#define MAX_PT_ENTRIES 32 // max entries in a Process Table

/** 
 * @param argv - a pointer to the char* argument array
 * @param input - same content as argv, but stored as a single string
 * @param arg_count - the number of arguments in the command
 * @param table - a reference to the process table class
 */
void execute_command(char** argv, string input, int arg_count, Process_Table& table);

/** 
 * print the user and system time used for this process and completed childs
 */
void print_resource();

/** 
 * execute the command with arguments in the argv array
 * @param argv - a pointer to the char* argument array
 * @param input - same content as argv, but stored as a single string
 * @param arg_count - the number of arguments in the command
 * @param table - a reference to the process table class
 */
void shell379_exec(char** argv, string input, int arg_count, Process_Table& table);

/** 
 * exit the shell, it will kill all running processes and print out the resource usage before exiting
 * @param table - a reference to the process table class
 */
void shell379_exit(Process_Table& table);

/**
 * print out the running processes along with the resource usage
 * it will use ps with pipe to grep to obtain the CPU time for each of the process
 * @param table - a reference to the process table class
 */
void shell379_jobs(Process_Table& table);

/** 
 * kill a process specified in the argument, or print out an error message if process cannot be found in process table
 * @param pid - the process id to kill
 * @param table - a reference to the process table class
 */
void shell379_kill(pid_t pid, Process_Table& table);

/**
 * resume a process specified in the argument, or print out an error message if process cannot be found in process table
 * @param table - a reference to the process table class 
 */
void shell379_resume(pid_t pid, Process_Table& table);


/** 
 * sleeper for the specified amount of seconds 
 * @param seconds - the number of seconds to sleep for
 */
void shell379_sleep(int seconds);

/** 
 * suspend a process specified in the argument, or print out an error message if process cannot be found in process table
 * @param pid - the process id to suspend
 * @param table - a reference to the process table class
 */
void shell379_suspend(pid_t pid, Process_Table& table);

/** 
 * wait for a process to complete execution
 * @param pid - the process id to wait for
 */
void shell379_wait(pid_t pid, Process_Table& table);

#endif