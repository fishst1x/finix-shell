/*
Name: Ryan Fisher
Date: 11-14-2025

Description: finix_parse.c is a very lengthy and beefy program.
It is the parsing engine for all commands and file input.
*/
#include "finix.h"
#include "finix_builtin.h"
#include "finix_utility.h"

#define MAX_TOKENS 32

// Forward Declares here
static int handle_command(char *cmd);
static int tokenize_with_quotes(char *cmd, char **argv, int max_tokens, int *background);

// Pase line using semicolon as delimiter.
int parse_semicolon(char *input) {
    char *saveptr = NULL;
    int should_exit = 0;

    char *segment = strtok_r(input, ";", &saveptr);

    while (segment) {
        // Trim leading whitespace
        while (*segment && isspace((unsigned char)*segment)) {
            segment++;
        }

        // Trim trailing whitespace
        char *end = segment + strlen(segment);
        while (end > segment && isspace((unsigned char)*(end - 1))) {
            end--;
        }
        *end = '\0';

        // Skip empty commands
        if (*segment != '\0') {
            int exit_here = handle_command(segment);
            if (exit_here) {
                    // Let other commands run and then exit. 
                should_exit = 1;
            }
        }

        segment = strtok_r(NULL, ";", &saveptr);
    }

    return should_exit;
}

static int handle_command(char *cmd) {
    char *argv[MAX_TOKENS];
    int argc = 0;
    int background = 0;

    argc = tokenize_with_quotes(cmd, argv, MAX_TOKENS, &background);
    if (argc == 0) {
        return 0;
    }

    // ---- built-ins ----

    // exit
    if (strcmp(argv[0], "exit") == 0) {
        return builtin_exit(argc, argv);      // Shell should exit on 1
    }

    // cd
    if (strcmp(argv[0], "cd") == 0) {
        return builtin_cd(argc, argv);
    }

    // myhistory
    if (strcmp(argv[0], "myhistory") == 0) {
        return builtin_myhistory(argc, argv);
    }

    // path
    if (strcmp(argv[0], "path") == 0) {
        return builtin_path(argc, argv);
    }

    // For all non biultins
    execute_command(argv, background);
    return 0;
}

// Tokenize a command line into argv[] with basic quote handling and bg '&'.
//
// Rules/simplifications:
//  - Whitespace separates tokens, *unless* inside quotes.
//  - "..." or '...' group into a single token; quotes are removed.
//  - '&' is treated specially *only* if it appears as the last token.
//  - Operators like <, >, >>, | should be separated by spaces for best results.
static int tokenize_with_quotes(char *cmd, char **argv, int max_tokens, int *background) {
    int argc = 0;
    *background = 0;

    char *p = cmd;

    while (*p) {
        // Skip leading whitespace
        while (*p && isspace((unsigned char)*p)) {
            p++;
        }
        if (!*p) {
            break;
        }

        // Start of a token
        char *start = p;
        char quote = 0;

        if (*p == '"' || *p == '\'') {
            // Quoted token
            quote = *p;
            start = ++p;  // skip opening quote

            while (*p && *p != quote) {
                p++;
            }

            if (*p == quote) {
                // Terminate token here
                *p = '\0';
                p++;
            }
        } else {
            // Unquoted token: read until whitespace
            while (*p && !isspace((unsigned char)*p)) {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
        }

        if (argc < max_tokens - 1) {
            argv[argc++] = start;
        } else {
            // Too many args – per assignment, we *do not* print an error.
            // We just truncate.
            break;
        }
    }

    // Check for background '&' as last token
    if (argc > 0 && strcmp(argv[argc - 1], "&") == 0) {
        *background = 1;
        argv[argc - 1] = NULL;
        argc--;
    }

    argv[argc] = NULL;
    return argc;
}