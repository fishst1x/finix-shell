/*
Name: Ryan Fisher
Date: 11-14-2025

Description: finix_builtin.h contains all prototypes
for builtin commands.
*/
#ifndef FINIX_BUILTIN_H
#define FINIX_BUILTIN_H

// Built-in command prototypes
int builtin_exit(int argc, char **argv);
int builtin_cd(int argc, char **argv);
int builtin_myhistory(int argc, char **argv);
int builtin_path(int argc, char **argv);

// Helper to put things into history buffer.
void history_add(const char *line);

#endif