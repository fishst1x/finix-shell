/*
Name: Ryan Fisher
Date: 11-14-2025

Description: finix.c handles the coordination of the entire program
including logic for deciding interactive/batch mode.
*/

#include "finix.h"
#include "finix_builtin.h"

int main(int argc, char *argv[]) {
    animate_finix();

    // Decide whether we should use interactive or batch here.
    if (argc == 1) {
        interactive_loop();
    } else if (argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        if (!fp) {
            fprintf(stderr, "finix: cannot open batch file '%s'\n", argv[1]);
            return 1;
        }
        finix_batch(fp);
        fclose(fp);
    } else {
        fprintf(stderr, "finix: usage: finix [batchfile]\n");
        return 1;
    }

    return 0;
}