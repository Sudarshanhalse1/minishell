---

# Mini Shell

## Description
Mini Shell is a simple shell program for Linux written in C.  
It lets you run commands like a normal terminal, handles signals, and supports basic built-in commands.

## Features
- Custom shell prompt (changeable with PS1)
- Runs external commands like ls, date, sleep
- Built-in commands: cd, pwd, exit
- Shows special variables: $$ (PID), $? (exit status), $SHELL
- Handles Ctrl+C and Ctrl+Z signals
- Supports command piping (e.g. ls -l | grep txt)

## Tech Stack
- C Programming
- Linux System Calls (fork, exec, wait, pipe, chdir)

## How to Run
1. Compile:
   ```bash
   gcc minishell.c -o minishell
2. Run:
   ```bash
   ./minishell

---
