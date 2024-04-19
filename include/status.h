#ifndef STATUS_H
#define STATUS_H

#include <stdbool.h>

#include "block.h"
#include "main.h"
#include "util.h"
#include "x11.h"
#include "tomlc.h"

typedef struct {
    char *current;
    char *previous;

    block **blocks;
    const unsigned short block_count;

    int status_sz;
} status;

status status_new(block **blocks, const unsigned short block_count);
bool status_update(status* const status);
int status_write(const status* const status, const bool is_debug_mode,
                 x11_connection* const connection);

#endif  // STATUS_H
