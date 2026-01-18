/*
 * Shell Header - Command line interface
 */

#ifndef SHELL_H
#define SHELL_H

#include "kernel.h"

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGS 10

void shell_init(void);
void shell_run(void);
void shell_print_prompt(void);
void shell_execute(const char* command);

#endif
