/*
Name: Ryan Fisher
Date: 11-14-2025

Description: finix_utility.c handles all external linux
utility commands. It is the engine for things like
pipe, <> >>.
*/
#include "finix.h"
#include "finix_utility.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_TOKENS 32
#define MAX_CMDS   3   // up to 2 pipes: cmd1 | cmd2 | cmd3

static int run_single_command(char **argv, int background);
static int run_pipeline(char ***cmds, int num_cmds, int background);

// For non biultin commands
int execute_command(char **argv, int background) {
    // Detect pipes
    int pipe_count = 0;

    for (int i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "|") == 0) {
            if (pipe_count < MAX_CMDS - 1) {
                pipe_count++;
            } else {
                // Too many pipes
                // Just ignore this command line.
                return 0;
            }
        }
    }

    if (pipe_count == 0) {
        // No pipes -> just a single command with optional redirection
        return run_single_command(argv, background);
    }

    // We have 1 or 2 pipes: split argv into up to MAX_CMDS commands.
    char *cmd_argvs[MAX_CMDS][MAX_TOKENS];
    int cmd_index = 0;
    int arg_index = 0;

    for (int i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "|") == 0) {
            // End command
            cmd_argvs[cmd_index][arg_index] = NULL;
            cmd_index++;
            arg_index = 0;
            if (cmd_index >= MAX_CMDS) {
                // Silient ignore
                return 0;
            }
        } else {
            if (arg_index < MAX_TOKENS - 1) {
                cmd_argvs[cmd_index][arg_index++] = argv[i];
            }
        }
    }
    cmd_argvs[cmd_index][arg_index] = NULL;
    int num_cmds = cmd_index + 1;

    // Ignore pipeline if any parts are empty
    for (int i = 0; i < num_cmds; i++) {
        if (cmd_argvs[i][0] == NULL) {
            return 0;
        }
    }

    // Run the pipeline.
    char **cmd_ptrs[MAX_CMDS];
    for (int i = 0; i < num_cmds; i++) {
        cmd_ptrs[i] = cmd_argvs[i];
    }
    return run_pipeline(cmd_ptrs, num_cmds, background);
}

// ---------------------------------------------
// Single command with optional <, >, >>
// ---------------------------------------------
// ---------------------------------------------
// Single command with optional <, >, >>
// ---------------------------------------------
static int run_single_command(char **argv, int background) {
    char *infile = NULL;
    char *outfile = NULL;
    int append = 0;

    // put tokens in array
    char *clean_argv[MAX_TOKENS];
    int j = 0;

    for (int i = 0; argv[i] != NULL && j < MAX_TOKENS - 1; i++) {
        if (strcmp(argv[i], "<") == 0) {
            // Ignore if no filename
            if (argv[i + 1] == NULL) {
                break;
            }
            infile = argv[++i];  // skip filename
        } else if (strcmp(argv[i], ">") == 0) {
            if (argv[i + 1] == NULL) {
                break;
            }
            outfile = argv[++i];
            append = 0;
        } else if (strcmp(argv[i], ">>") == 0) {
            if (argv[i + 1] == NULL) {
                break;
            }
            outfile = argv[++i];
            append = 1;
        } else {
            clean_argv[j++] = argv[i];
        }
    }
    clean_argv[j] = NULL;

    if (clean_argv[0] == NULL) {
        // Nothing to execute
        return 0;
    }

    pid_t pid = fork();
    if (pid < 0) {
        // ignore fork failed
        return 0;
    }

    if (pid == 0) {
        // Child process

        // ---------- INPUT REDIRECT (<) ----------
        if (infile != NULL) {
            int fd_in = open(infile, O_RDONLY);
            if (fd_in >= 0) {
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            // If open fails, run without redirect and donn't error
        }

        // ---------- OUTPUT REDIRECT (>, >>) ----------
        if (outfile != NULL) {
            int flags = O_WRONLY | O_CREAT;
            if (append) {
                flags |= O_APPEND;
            } else {
                flags |= O_TRUNC;
            }

            int fd_out = open(outfile, flags, 0666);
            if (fd_out >= 0) {
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            // If open fails, run without redirect and donn't error
        }

        execvp(clean_argv[0], clean_argv);

        // execvp failure
        fprintf(stderr,
                "The command %s does not exist or could not be executed.\n",
                clean_argv[0]);
        _exit(1);
    }

    if (background) {
        // don't wait
        return 0;
    }

    // wait for child
    int status;
    pid_t w;
    do {
        w = waitpid(pid, &status, 0);
    } while (w == -1 && errno == EINTR);

    return 0;
}

// ---------------------------------------------
// Pipelines: cmd1 | cmd2 [| cmd3]
// Redirection:
//   - you can use < on the first command
//   - you can use > or >> on the last command
//   e.g.,  sort < in.txt | uniq | wc >> out.txt
// ---------------------------------------------
static int run_pipeline(char ***cmds, int num_cmds, int background) {
    int pipes[2][2]; // up to 2 pipes
    pid_t pids[MAX_CMDS];

    char *infile = NULL;
    char *outfile = NULL;
    int append = 0;

    // For each command, build string with redirect symbols
    char *clean_cmds[MAX_CMDS][MAX_TOKENS];

    for (int c = 0; c < num_cmds; c++) {
        int j = 0;
        for (int i = 0; cmds[c][i] != NULL && j < MAX_TOKENS - 1; i++) {
            if (c == 0 && strcmp(cmds[c][i], "<") == 0) {
                if (cmds[c][i + 1] == NULL) {
                    // no filename
                    break;
                }
                infile = cmds[c][++i];
            } else if (c == num_cmds - 1 && strcmp(cmds[c][i], ">") == 0) {
                if (cmds[c][i + 1] == NULL) {
                    break;
                }
                outfile = cmds[c][++i];
                append = 0;
            } else if (c == num_cmds - 1 && strcmp(cmds[c][i], ">>") == 0) {
                if (cmds[c][i + 1] == NULL) {
                    break;
                }
                outfile = cmds[c][++i];
                append = 1;
            } else {
                clean_cmds[c][j++] = cmds[c][i];
            }
        }
        clean_cmds[c][j] = NULL;
        if (clean_cmds[c][0] == NULL) {
            // Empty command in pipeline; silently skip whole pipeline
            return 0;
        }
    }

    // Create pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            
            return 0;
        }
    }

    // Fork each command
    for (int c = 0; c < num_cmds; c++) {
        pids[c] = fork();

        if (pids[c] < 0) {
            // fork failed; close pipes
            for (int i = 0; i < num_cmds - 1; i++) {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            return 0;
        }

    if (pids[c] == 0) {
        // Child c

        // ---------- INPUT: either from previous pipe OR infile ----------
        if (c > 0) {
            // Not first command: stdin from previous pipe
            dup2(pipes[c - 1][0], STDIN_FILENO);
        } else if (infile != NULL) {
            // First command: maybe stdin from file
            int fd_in = open(infile, O_RDONLY);
            if (fd_in >= 0) {
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            // If open fails, run without redirect; no shell error.
        }

        // ---------- OUTPUT: either to next pipe OR outfile ----------
        if (c < num_cmds - 1) {
            // Not last command: stdout to next pipe
            dup2(pipes[c][1], STDOUT_FILENO);
        } else if (outfile != NULL) {
            // Last command: maybe stdout to file
            int flags = O_WRONLY | O_CREAT;
            if (append) {
                flags |= O_APPEND;
            } else {
                flags |= O_TRUNC;
            }

            int fd_out = open(outfile, flags, 0666);
            if (fd_out >= 0) {
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            // If open fails, run without redirect; no shell error.
        }

        // Close all pipe fds in child
        for (int i = 0; i < num_cmds - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        execvp(clean_cmds[c][0], clean_cmds[c]);
        fprintf(stderr,
                "The command %s does not exist or could not be executed.\n",
                clean_cmds[c][0]);
        _exit(1);
    }

    // Close all pipe fds in child
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    execvp(clean_cmds[c][0], clean_cmds[c]);
    fprintf(stderr,
            "The command %s does not exist or could not be executed.\n",
            clean_cmds[c][0]);
    _exit(1);
}

    // Parent: close all pipe fds
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    if (background) {
        // Background pipeline: per sample, no extra shell messages.
        return 0;
    }

    // Foreground: wait for all children
    for (int c = 0; c < num_cmds; c++) {
        int status;
        pid_t w;
        do {
            w = waitpid(pids[c], &status, 0);
        } while (w == -1 && errno == EINTR);
    }

    return 0;
}