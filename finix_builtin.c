/*
Name: Ryan Fisher
Date: 11-14-2025

Description: finix_builtin.h handles all
builtin commands.
*/
#include "finix.h"
#include "finix_builtin.h"
#include <stdlib.h>

// ======================
//  exit builtin
// ======================

// Return 1 to exit
int builtin_exit(int argc, char **argv) {
    (void)argc;
    (void)argv;
    return 1;
}

// ======================
//  cd builtin
// ======================

int builtin_cd(int argc, char **argv) {
    const char *path = NULL;

    if (argc < 2) {
        // go to $HOME if no argument
        path = getenv("HOME");
        if (path == NULL) {
            // safetynet check if home does not exist.
            path = ".";
        }
    } else {
        // Use the provided path
        path = argv[1];
    }

    if (chdir(path) != 0) {
        // print of cd command fails
        perror("cd");
    }

    return 0;
}

// ======================
//  myhistory internals
// ======================

#define HISTORY_MAX 20

static char *history[HISTORY_MAX];
static int history_count = 0;
static int history_start = 0;

// Record a new command into history buffer.
void history_add(const char *line) {
    if (!line || line[0] == '\0') {
        return;  // ignore empty lines
    }

    char *copy = strdup(line);
    if (!copy) {
        return; 
    }

    if (history_count < HISTORY_MAX) {
        int idx = (history_start + history_count) % HISTORY_MAX;
        history[idx] = copy;
        history_count++;
    } else {
        // overwrite oldest command if we have 20
        int idx = history_start;
        free(history[idx]);
        history[idx] = copy;
        history_start = (history_start + 1) % HISTORY_MAX;
    }
}

// Clear all history entries.
static void history_clear(void) {
    for (int i = 0; i < HISTORY_MAX; i++) {
        if (history[i] != NULL) {
            free(history[i]);
            history[i] = NULL;
        }
    }
    history_count = 0;
    history_start = 0;
}

// Print the history from oldest to newest.
static void history_print(void) {
    printf("Command History\n");

    for (int i = 0; i < history_count; i++) {
        int idx = (history_start + i) % HISTORY_MAX;
        printf("[%d] %s\n", i + 1, history[idx]);
    }
}

// Get a specific history entry.
static const char *history_get(int n) {
    if (n < 1 || n > history_count) {
        return NULL;
    }
    int idx = (history_start + (n - 1)) % HISTORY_MAX;
    return history[idx];
}

// ======================
//  myhistory builtin
// ======================
//
//  myhistory               -> list history
//  myhistory -c            -> clear
//  myhistory -e <number>   -> execute that entry
//
int builtin_myhistory(int argc, char **argv) {
    if (argc == 1) {
        //print history
        history_print();
        return 0;
    }

    if (argc == 2 && strcmp(argv[1], "-c") == 0) {
        // Clear history
        history_clear();
        return 0;
    }

    if (argc == 3 && strcmp(argv[1], "-e") == 0) {
        char *endptr = NULL;
        long n = strtol(argv[2], &endptr, 10);

        if (*argv[2] == '\0' || *endptr != '\0' || n <= 0 || n > history_count) {
            fprintf(stderr, "myhistory: invalid entry number '%s'\n", argv[2]);
            return 0;
        }

        const char *cmdline = history_get((int)n);
        if (!cmdline) {
            fprintf(stderr, "myhistory: no such entry %ld\n", n);
            return 0;
        }

        // Add the command line again to history
        history_add(cmdline);

        // give copy to parser
        char *copy = strdup(cmdline);
        if (!copy) {
            perror("myhistory");
            return 0;
        }

        // Same logic
        int should_exit = parse_semicolon(copy);

        free(copy);

        
        if (should_exit) {
            return 1;
        }

        return 0;
    }

    fprintf(stderr, "myhistory: usage: myhistory | myhistory -c | myhistory -e <number>\n");
    return 0;
}

// ======================
//  path builtin
// ======================
//
//  path                 -> show current path list
//  path + <dir>         -> append directory to path list
//  path - <dir>         -> remove directory from path list
//
//  Internally, we keep our own list of path entries and also sync it back to
//  the PATH environment variable so that execvp() will use the updated PATH.
//

#define PATH_MAX_ENTRIES 64

static char *path_entries[PATH_MAX_ENTRIES];
static int path_count = 0;
static int path_initialized = 0;

static void path_init(void) {
    if (path_initialized) {
        return;
    }

    char *env = getenv("PATH");
    if (!env) {
        // No PATH set; start with empty list
        path_count = 0;
        path_initialized = 1;
        return;
    }

    // Make a copy of PATH to tokenize
    char *copy = strdup(env);
    if (!copy) {
        path_count = 0;
        path_initialized = 1;
        return;
    }

    char *saveptr = NULL;
    char *tok = strtok_r(copy, ":", &saveptr);

    while (tok && path_count < PATH_MAX_ENTRIES) {
        path_entries[path_count++] = strdup(tok);
        tok = strtok_r(NULL, ":", &saveptr);
    }

    free(copy);
    path_initialized = 1;
}

// Rebuild the PATH environment variable from path_entries[]
static void path_sync_env(void) {
    if (path_count == 0) {
        // Set PATH to empty string
        setenv("PATH", "", 1);
        return;
    }

    // Compute total length needed
    size_t total_len = 0;
    for (int i = 0; i < path_count; i++) {
        total_len += strlen(path_entries[i]);
        if (i < path_count - 1) {
            total_len += 1; // for ':'
        }
    }

    char *buf = malloc(total_len + 1);
    if (!buf) {
        return; // fail silently
    }

    char *p = buf;
    for (int i = 0; i < path_count; i++) {
        size_t len = strlen(path_entries[i]);
        memcpy(p, path_entries[i], len);
        p += len;
        if (i < path_count - 1) {
            *p++ = ':';
        }
    }
    *p = '\0';

    setenv("PATH", buf, 1);
    free(buf);
}

// Remove a path entry equal to 'dir'; returns 1 if removed, 0 if not found.
static int path_remove_entry(const char *dir) {
    for (int i = 0; i < path_count; i++) {
        if (strcmp(path_entries[i], dir) == 0) {
            free(path_entries[i]);
            // Shift the remaining entries down
            for (int j = i; j < path_count - 1; j++) {
                path_entries[j] = path_entries[j + 1];
            }
            path_count--;
            return 1;
        }
    }
    return 0;
}

// Append dir if not already present
static void path_append_entry(const char *dir) {
    // Check if it already exists
    for (int i = 0; i < path_count; i++) {
        if (strcmp(path_entries[i], dir) == 0) {
            // Already there; nothing to do
            return;
        }
    }

    if (path_count >= PATH_MAX_ENTRIES) {
        fprintf(stderr, "path: maximum number of path entries reached\n");
        return;
    }

    path_entries[path_count++] = strdup(dir);
}

// Built-in path implementation
int builtin_path(int argc, char **argv) {
    path_init();

    if (argc == 1) {
        // No arguments: display current path entries joined with colons
        if (path_count == 0) {
            printf("\n");
            return 0;
        }
        for (int i = 0; i < path_count; i++) {
            printf("%s", path_entries[i]);
            if (i < path_count - 1) {
                printf(":");
            }
        }
        printf("\n");
        return 0;
    }

    if (argc != 3) {
        fprintf(stderr, "path: usage:\n");
        fprintf(stderr, "  path\n");
        fprintf(stderr, "  path + <directory>\n");
        fprintf(stderr, "  path - <directory>\n");
        return 0;
    }

    const char *op = argv[1];
    const char *dir = argv[2];

    if (strcmp(op, "+") == 0) {
        path_append_entry(dir);
        path_sync_env();
        return 0;
    } else if (strcmp(op, "-") == 0) {
        if (!path_remove_entry(dir)) {
            fprintf(stderr, "path: directory '%s' not found in path\n", dir);
        }
        path_sync_env();
        return 0;
    } else {
        fprintf(stderr, "path: unknown operator '%s' (use + or -)\n", op);
        return 0;
    }
}