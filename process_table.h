/** 
 * CMPUT 379 Fall 2020 Assignment 1
 * Name: Ze Hui Peng
 * ccid: zhpeng
 * id: 1594884
 * 
 * process_table.h
 */

#ifndef _PROCESS_TABLE_H
#define _PROCESS_TABLE_H

#include <algorithm> // count()
#include <iostream> // cout
#include <stdlib.h> // atoi()
#include <string>
#include <string.h> // strcmp(), strlen(), strtok()
#include <sys/wait.h> // waitpid()
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

// index constants defined for the value vector of the unordered_map
#define COMMAND_INDEX 0
#define TIME_INDEX 1
#define STATUS_INDEX 2

class Process_Table {
public:
    /** 
     * Add a process to the process table
     * @param pid - the process id of the process
     * @param cmd - the command that this process is executing
     */
    void add_process(int pid, string cmd);

    /**
     * change the state of a process in the process table
     * @param pid - the process id of the process
     * @param state - the state of the process to change, should either be "R" or "T" for the assignment
     */
    void change_state(int pid, string state);

    /** 
     * check for each process in the process table to see if it is still active(either running or suspended)
     * if the process is no longer active, remove it from the table
     */
    void check_process();

    /**
     * find if the specified pid exist in the process table
     * @param pid - the process id of the process
     * @return true if the process exist, false if it does not
     */
    bool find_pid(int pid);

    /** 
     * get the number of entries currently exist in the process table
     * @return the number of entries in the process table as a integer
     */
    int get_num_entries();

    /**
     * get an iterator to the start of the table
     */
    unordered_map<int, vector<string> >::const_iterator get_start_iterator();

    /**
     * get an iterator to the end of the table
     */
    unordered_map<int, vector<string> >::const_iterator get_end_iterator();

    /**
     * print the table to stdout in a specific format
     */
    void print_table();

    /**
     * remove the specific process from the process table
     * @param pid - the process id of the process
     */
    void remove_process(int pid);

    /** 
     * update the table with the provided "time" table by iterating over all processes
     * @param pid_to_time - an unordered_map that contains the process id as the key and the cumulative CPU time that associated 
     *                      with the process as value
     */
    void update_table(unordered_map<int,string> pid_to_time);

    /**
     * update the "time" field of the vector with the value in the provided unordered_map
     * @param pid - the process id of the process
     * @param time - the process's CPU time in a string of format "0:00:00" or "0:00"
     */
    void update_time(int pid, string time);

private:
    unordered_map<int, vector<string> > process_table;
};

#endif