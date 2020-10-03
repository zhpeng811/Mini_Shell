/** 
 * CMPUT 379 Fall 2020 Assignment 1
 * Name: Ze Hui Peng
 * ccid: zhpeng
 * id: 1594884
 * 
 * shell379.cpp
 */

#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>

#include "command_executer.h"

using namespace std;

/** 
 * parse the string input into a char* array, as well as validating the input
 * @param input - the input in string format
 * @param argv - a pointer to a char* array, where the parsed input will be stored at 
 * @return the number of arguments in the input
 *         or -1 if the input does not match the requirements
 */
int parseInput(string input, char** argv) {
    char *str = new char [input.length() + 1];
    strcpy(str, input.c_str());

    char *arg = strtok(str, " ");
    int argv_pointer = 0;
    while (arg != NULL) {
        if (strlen(arg) > MAX_LENGTH) {
            cout << "an argument have too much characters, the maximum length of an argument is " << MAX_LENGTH << "\n";
            return -1; 
        } else if (argv_pointer > MAX_ARGS) {
            cout << "Too many arguments, the maximum arguments allowed is " << MAX_ARGS << "\n";
            return -1;
        }
        argv[argv_pointer] = arg;
        arg = strtok(NULL, " ");
        argv_pointer++;
    }
    argv[argv_pointer] = NULL;

    // argv[0] is the command, therefore it does not count towards an argument
    return argv_pointer - 1;
}

/** 
 *  Main function, reponsible for getting user input and calling necessary function to parse and execute the input
 */
int main() {
    Process_Table table;
    char *argv[MAX_ARGS + 2]; // + 1 for the command itself, + 1 for NULL terminator
    bool shell_running = true;

    // look forever until user types "exit"
    while(shell_running) {
        cout << "SHELL379: ";
        string input;
        getline(cin, input); // will automatically wait for user input

        table.check_process(); // this will check for each process in the table to see if it is still active 
        if (input.length() == 0) {
            continue;
        } 
        else if (input.length() > LINE_LENGTH) {
            cout << "Input length too long, the maximum length allowed is " << LINE_LENGTH << "\n";
            continue;
        } 
        else if (table.get_num_entries() == MAX_PT_ENTRIES) {
            cout << "Process table reached max entries, please kill a process or wait for processes to finish before continuing to execute commands" << "\n";
            continue;
        }

        int arg_count = parseInput(input, argv);
        if (arg_count == -1) {
            continue;
        }

        execute_command(argv, input, arg_count, table);
        if (input.compare("exit") == 0) {
            shell_running = false;
        }
    }

    return 0;
}