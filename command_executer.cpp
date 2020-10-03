/** 
 * CMPUT 379 Fall 2020 Assignment 1
 * Name: Ze Hui Peng
 * ccid: zhpeng
 * id: 1594884
 * 
 * command_executer.cpp
 */

#include "command_executer.h"

void execute_command(char** argv, string input, int arg_count, Process_Table& table) {
    char* cmd = argv[0];
    try {
        // first check out if an argument exist for commands that requires an argument
        if (
            (strcmp(cmd, "kill") == 0 ||
            strcmp(cmd, "resume") == 0 ||
            strcmp(cmd, "wait") == 0 ||
            strcmp(cmd, "suspend") == 0 ||
            strcmp(cmd, "sleep") == 0) &&
            arg_count != 1
        ) {
            cout << "Invalid number of arguments for " << cmd << "\n";
            return;
        }

        if (strcmp(cmd, "exit") == 0) {
            shell379_exit(table);
        } 
        else if (strcmp(cmd, "jobs") == 0) {
            shell379_jobs(table);
        } 
        else if (strcmp(cmd, "kill") == 0) {
            pid_t pid = (pid_t) atoi(argv[1]);
            shell379_kill(pid, table);
        } 
        else if (strcmp(cmd, "resume") == 0) {
            pid_t pid = (pid_t) atoi(argv[1]);
            shell379_resume(pid, table);
        } 
        else if (strcmp(cmd, "wait") == 0) {
            pid_t pid = (pid_t) atoi(argv[1]);
            shell379_wait(pid, table);
        } 
        else if (strcmp(cmd, "sleep1") == 0) {
            int arg_second = atoi(argv[1]);
            shell379_sleep(arg_second);
        } 
        else if (strcmp(cmd, "suspend") == 0) {
            pid_t pid = (pid_t) atoi(argv[1]);
            shell379_suspend(pid, table);
        } 
        else {
            shell379_exec(argv, input, arg_count, table);
        }

    } catch (...) {
        cout << "input error\n";
    }

}


void print_resource() {
    struct rusage usage;
    // RUSAGE_CHILDREN will obtain the usage time for all finished child processes and store it into the usage struct
    getrusage(RUSAGE_CHILDREN, &usage);

    cout << "Resources used:\n";
    cout << "User time =        " << usage.ru_utime.tv_sec << " seconds \n";
    cout << "System time =      " << usage.ru_stime.tv_sec << " seconds \n";
}


void shell379_exec(char** argv, string input, int arg_count, Process_Table& table) {
    int in_fd = -1;
    int out_fd = -1;
    int stdin_fd = dup(STDIN_FILENO); // save a record of stdin's file #
    int stdout_fd = dup(STDOUT_FILENO); // save a record of stdout's file #
    bool run_bg = !strcmp(argv[arg_count], "&");
    
    // parsing argv for <, and > symbol before continuing
    for (int i = 1; i <= arg_count; i++) {
        char *arg = argv[i];
        if (arg[0] == '<') {
            if (strlen(arg) == 1) {
                // handle the format "< input.txt"(although not required for this assignment)
                argv[i] = NULL;
                arg = argv[i+1]; // next argument is the file name
                i++;
            } 
            else {
                // handle the format "<input.txt"
                arg += 1; // increment by pointer by 1 to not include '<'
            }
            in_fd = open(arg, O_RDWR);
            if (in_fd == -1) {
                cout << "error in processing input file" << endl;
                return;
            }
            dup2(in_fd, STDIN_FILENO); // redirect stdin to the input file's file descriptor 
            argv[i] = NULL; // clear the input file's argument in argv
        } 
        else if (arg[0] == '>') {
            if (strlen(arg) == 1) {
                // handle the format "> output.txt"(although not required for this assignment)
                argv[i] = NULL;
                arg = argv[i+1];
                i++;
            } 
            else {
                // handle the format ">output.txt"
                arg += 1;  // increment by pointer by 1 to not include '>'
            }
            out_fd = open(arg, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
            if (out_fd == -1) {
                cout << "error in processing output file" << endl;
                return;
            }
            dup2(out_fd, STDOUT_FILENO); // redirect stdout to the output file's file descriptor 
            argv[i] = NULL;  // clear the input file's argument in argv
        }
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("error with fork, please try again");
    } 
    else if (pid == 0) { // child process
        if (execvp(argv[0], argv) < 0) {
            perror("error with execvp, please try again");
            _exit(0); // needed when execvp fails
        }
    } 
    else { // parent process
        if (!run_bg) {
            wait(NULL);
        }
        if (in_fd != -1) {
            // close the file descriptor and redirect stdin to it's original file descriptor
            close(in_fd);
            dup2(stdin_fd, STDIN_FILENO);
        } 
        if (out_fd != -1) {
            // close the file descriptor and redirect stdout to it's original file descriptor
            close(out_fd);
            dup2(stdout_fd, STDOUT_FILENO);
        }
        table.add_process((int) pid, input);
    }
}


void shell379_exit(Process_Table& table) {
    // iterate through the process table and kill all active processes before printing the resource
    for (auto iter = table.get_start_iterator(); iter != table.get_end_iterator(); iter++) {
        int pid = iter->first;
        kill(pid, SIGKILL);
    }
    print_resource();
}


void shell379_jobs(Process_Table& table) {
    FILE *ps_file;
    FILE *grep_file;
    char buffer[LINE_LENGTH];
    string line;
    unordered_map<int, string> pid_to_time;
    int stdin_fd = dup(STDIN_FILENO); // save a copy of stdin's file descriptor

    ps_file = popen("ps -o pid,time,pgid", "r");
    if (ps_file == NULL) {
        perror("error with popen");
        return;
    }
    int ps_fd = fileno(ps_file);
    dup2(ps_fd, STDIN_FILENO); // redirect stdin to the output of the popen() command above

    int parent_pid = (int) getpid();
    // grep on the parent pid to only retrieve processes that belongs to the specific parent group
    string grep_arg = "grep " + to_string(parent_pid);
    grep_file = popen(grep_arg.c_str(), "r");
    if (grep_file == NULL) {
        perror("error with popen");
        return;
    }

    // parse the output, the shell process itself and the grep process will be included
    // but the purpose is to update the cumulative CPU time for existing processes in the process table
    // so although the above two processes are included it will not affect the process table
    while (!feof(grep_file)) {
        if (fgets(buffer, sizeof(buffer), grep_file) == NULL) {
            continue;
        }
        line.append(buffer);
        int index = line.find(" "); // index of the first element's start position (i.e. pid)
        int index2 = line.find(" ", index + 1); // index of the second element's start position (i.e. time)
        int pid = stoi(line.substr(0, index));
        string time = line.substr(index + 1, index2);
        time = time.substr(time.find_first_not_of(" ")); // trim leading white spaces
        pid_to_time[pid] = time;
        line.clear();
    }

    table.update_table(pid_to_time);
    
    // close all file descriptors and redirect stdin to its original file descriptor
    pclose(ps_file);
    pclose(grep_file);
    close(ps_fd);
    dup2(stdin_fd, STDIN_FILENO);
    close(stdin_fd);

    // print the table along with the resource usage
    table.print_table();
    print_resource();
}


void shell379_kill(pid_t pid, Process_Table& table) {
    if (!table.find_pid(pid)) {
        return;
    }
    
    int kill_status = kill(pid, SIGKILL);
    if (kill_status == 0) {
        table.remove_process((int) pid);
    } else {
        perror("error in killing process");
    }
}


void shell379_resume(pid_t pid, Process_Table& table) {
    if (!table.find_pid(pid)) {
        return;
    }

    int kill_status = kill(pid, SIGCONT);
    if (kill_status == 0) {
        table.change_state((int) pid, "R");
    } else {
        perror("error in resuming process");
    }
}


void shell379_sleep(int seconds) {
    sleep(seconds);
}


void shell379_suspend(pid_t pid, Process_Table& table) {
    if (!table.find_pid(pid)) {
        return;
    }

    int kill_status = kill(pid, SIGSTOP);
    if (kill_status == 0) {
        table.change_state((int) pid, "T");
    } else {
        perror("error in suspending process");
    }
}


void shell379_wait(pid_t pid, Process_Table& table) {
    if (!table.find_pid(pid)) {
        return;
    }
    
    int wait_status = (int) waitpid(pid, NULL, 0);
    if (wait_status < 0) {
        perror("error in waiting process");
    }
}