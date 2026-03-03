/*
Name: Ryan Fisher
Date: 11-14-2025

Description: finix_interactive.c handles the logic for running in interactive mode. 
Expects a expects a command(s) and while run into terminated with exit call.
*/
#include "finix.h"
#include "finix_builtin.h"

void interactive_loop(void) {
    char input[BUF_SIZE];

    while (1) {
        print_prompt();
        get_input(input);

        if (input[0] == '\0') {
            continue;   // ignore empty/EOF lines
        }

        // Record command in history
        history_add(input);

        int should_exit = parse_semicolon(input);
        if (should_exit) {
            // Terminate shell after EVERYTHING ELSE
            exit(0);
        }
    }
}