/*
Name: Ryan Fisher
Date: 11-14-2025

Description: finix_batch.c handles the logic for running in batch mode. 
Expects a file and will echo lines and run commands from file.
*/
#include "finix.h"
#include "finix_builtin.h"

void finix_batch(FILE *fp) {
    char line[BUF_SIZE];

    while (fgets(line, sizeof(line), fp) != NULL) {
        // Remove newlines just in case! This caught me up for hours!!!!
        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0') {
            continue;   // ignore blank lines
        }

        // Echo the line per instructions
        printf("%s\n", line);

        // Record command in history
        history_add(line);

        int should_exit = parse_semicolon(line);
        if (should_exit) {
            exit(0);
        }
    }
}