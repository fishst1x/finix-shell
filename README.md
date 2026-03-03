# ✅ Finix Shell

## About

Hello there and thank you for visiting my Finix Shell repository!!!

This C program mimics a lightweight linux shell that performs some of the most common linux functionalities. This is done by leveraging low-level system calls like fork(), exec(), wait(), waitpid(), and bitwise operations. The program is capable of the following native built-in functionality:

- ls
- cat
- alias
- exit
- pwd
- cd
- path
- Pipelining - "|"
- take in multiple commands on a single line
- myhistory (to retain finix shell built-in history, I had to avoid name clashing)
- Output redirection - ">", "<", "<<", ">>"

The program also features some decent error handling and resilience where it detects spaces, performs signal handling and command detection.


---

# Usage

## Build
`make`

or

`make finix`

## Show usage help
`make usage`

## Clean
`make clean`

### Program usage
The program can be ran using `.finix` or can be ran using a batch file of sample command `./finix <batch>`.


---

## 🗂 Project Organization

```text
- [x] finix.c
      - Entry point. Handles interactive vs batch mode selection.
- [x] finix.h
      - Shared declarations, constants (e.g., BUF_SIZE), prototypes.
- [x] finix_terminal.c
      - Prompt printing and user input (print_prompt, get_input).
- [x] finix_logo.c
      - (Optional) Finix startup animation / ASCII art.
- [x] finix_interactive.c
      - Interactive shell loop (interactive_loop).
- [x] finix_batch.c
      - Batch mode driver (finix_batch) for processing a batch file.
- [x] finix_parse.c
      - Parsing of input lines, semicolon splitting, tokenization with quotes,
        background '&' detection, and builtin dispatch.
- [x] finix_builtin.c
      - Implementations of builtins: exit, cd, myhistory, path, and history
        management (ring buffer of 20 commands).
- [x] finix_builtin.h
      - Prototypes for builtin functions and history_add.
- [x] finix_utility.c
      - Execution engine for non-builtin commands:
          * fork + execvp
          * I/O redirection: <, >, >>
          * Pipelines: cmd1 | cmd2 [| cmd3]
          * Optional background execution (&) for external commands.
- [x] finix_utility.h
      - Prototype for execute_command.
- [x] finix.batch
      - Sample batch file demonstrating batch mode (pwd, cd, path, pipes,
        redirection, myhistory, exit).
- [x] makefile
      - Builds the finix binary and provides a clean target.
```
---

## Contributors
 - Ryan Fisher
