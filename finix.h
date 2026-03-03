/*
Name: Ryan Fisher
Date: 11-14-2025

Description: The core header for the core part of the program.
*/
#ifndef FINIX_H
#define FINIX_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define BUF_SIZE 513   // 512 chars + NULL terminator

// animation
void animate_finix(void);

// terminal helpers
void print_prompt(void);
void get_input(char *input);

// parsing
int parse_semicolon(char *input);

// modes
void interactive_loop(void);
void finix_batch(FILE *fp);

#endif