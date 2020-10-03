/** 
 * CMPUT 379 Fall 2020 Assignment 1
 * Name: Ze Hui Peng
 * ccid: zhpeng
 * id: 1594884
 * 
 * process_table.cpp
 */

#include "process_table.h"

void Process_Table::add_process(int pid, string cmd) {
    vector<string> info;
    info.push_back(cmd);
    info.push_back("0"); // start time is 0
    info.push_back("R"); // when a process is just added, the state will always be 'R'(running)

    process_table[pid] = info;
}


void Process_Table::change_state(int pid, string state) {
    process_table[pid].at(STATUS_INDEX) = state;
}


void Process_Table::check_process() {
    auto iter = process_table.begin();
    while (iter != process_table.end()) {
        int process_pid = iter->first;
        pid_t return_pid = waitpid(process_pid, NULL, WNOHANG);
        if (return_pid == process_pid || return_pid == -1) { // child process no longer exist
            iter = process_table.erase(iter);
        } else {
            iter++;
        }
    }
}

bool Process_Table::find_pid(int pid) {
    if (process_table.find(pid) != process_table.end()) {
        return true;
    } else {
        cout << "Process with pid " << pid << " does not exist in the process table\n";
        return false;
    }
}


int Process_Table::get_num_entries() {
    return process_table.size();
}


unordered_map<int, vector<string> >::const_iterator Process_Table::get_start_iterator() {
    return process_table.begin();
}


unordered_map<int, vector<string> >::const_iterator Process_Table::get_end_iterator() {
    return process_table.end();
}


void Process_Table::print_table() {
    int process_count = 0;
    int active_count = 0;
    cout << "Running Processes:\n";
    if (get_num_entries() != 0) {
        cout << " #    " << "PID " << "S " << "SEC   " << "COMMAND\n";
    }
    for (auto iter = process_table.begin(); iter != process_table.end(); iter++) {
        int pid = iter->first;
        vector<string> info = iter->second;
        cout << process_count << ":  " << pid << " " << info.at(STATUS_INDEX) << " " << info.at(TIME_INDEX) << "   " << info.at(COMMAND_INDEX) << "\n";
        process_count++;
        if (info.at(STATUS_INDEX).compare("R") == 0) {
            active_count++;
        }
    }
    cout << "Processes =        " << active_count << " active\n";
}

void Process_Table::remove_process(int pid) {
    process_table.erase(pid);
}

void Process_Table::update_table(unordered_map<int,string> pid_to_time) {
    for (auto iter = process_table.begin(); iter != process_table.end(); iter++) {
        int process_pid = iter->first;
        if (pid_to_time.find(process_pid) != pid_to_time.end()) {
            update_time(process_pid, pid_to_time[process_pid]);
        }
    }
}

void Process_Table::update_time(int pid, string time) {
    int colon_count = count(time.begin(), time.end(), ':');
    char *str = new char [time.length() + 1];
    strcpy(str, time.c_str()); // convert string to char *

    char *num = strtok(str, ":");
    int seconds = 0;
    int multiplier = (colon_count == 2) ? 3600 : 60; // colon count = 2 -> time includes hours, = 1 -> time only include minutes
    while (num != NULL && multiplier > 0) {
        seconds += atoi(num) * multiplier;
        num = strtok(NULL, ":");
        multiplier /= 60;
    }

    process_table[pid].at(TIME_INDEX) = to_string(seconds);
}