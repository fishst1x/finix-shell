/*
Name: Ryan Fisher
Date: 11-14-2025

Description: finix_terminal.c holds logic for creating
prompt and getting initial input.
*/
#include "finix.h"

// Create the prompt
void print_prompt(void) {
    printf("finix@you> ");
    fflush(stdout);
}

// get_input helper script
void get_input(char *input) {
    if (!fgets(input, BUF_SIZE, stdin)) {
        input[0] = '\0';
        return;
    }
    input[strcspn(input, "\n")] = '\0';
}