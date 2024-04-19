#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include "util.h"

typedef struct {
    const char *command;
    unsigned int interval;
    int signal;

    int pipe[PIPE_FD_COUNT];
    char *output;
    int output_len;
    pid_t fork_pid;
} block;

block *block_new(const char *const command, const unsigned int interval,
                const int signal);
int block_init(block *block);
int block_deinit(block *block);
int block_execute(block *block, const uint8_t button);
int block_update(block *block);

#endif  // BLOCK_H
