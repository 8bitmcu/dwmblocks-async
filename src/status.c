#include "status.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "block.h"
#include "tomlc.h"
#include "util.h"
#include "x11.h"

static bool has_status_changed(const status *const status) {
    return strcmp(status->current, status->previous) != 0;
}

status status_new(block **blocks,
                  const unsigned short block_count) {
    status status = {
        .blocks = blocks,
        .block_count = block_count,
    };

    status.status_sz = ((n_blocks * max_block_output_length * UTF8_MAX_BYTE_COUNT + clickable_blocks) +
        (n_blocks - 1 + leading_delimiter + trailing_delimiter) * (delimiter_len - 1) + 1);
    status.current = malloc(status.status_sz * sizeof(char));
    status.previous = malloc(status.status_sz * sizeof(char));

    status.current[0] = '\0';
    status.previous[0] = '\0';

    return status;
}

bool status_update(status *const status) {
    (void)strncpy(status->previous, status->current, status->status_sz);
    status->current[0] = '\0';

    for (unsigned short i = 0; i < status->block_count; ++i) {
        block *block = status->blocks[i];

        if (strlen(block->output) > 0) {
            if (leading_delimiter)
              (void)strncat(status->current, delimiter, delimiter_len);
            else if (status->current[0] != '\0') {
                (void)strncat(status->current, delimiter, delimiter_len);
            }

            if (clickable_blocks && block->signal > 0) {
                const char signal[] = {(char)block->signal, '\0'};
                (void)strncat(status->current, signal, LEN(signal));
            }

            (void)strncat(status->current, block->output, block->output_len);
        }
    }

    if (trailing_delimiter && status->current[0] != '\0') {
        (void)strncat(status->current, delimiter, delimiter_len);
    }

    return has_status_changed(status);
}

int status_write(const status *const status, const bool is_debug_mode,
                 x11_connection *const connection) {
    if (is_debug_mode) {
        (void)printf("%s\n", status->current);
        return 0;
    }

    if (x11_set_root_name(connection, status->current) != 0) {
        return 1;
    }

    return 0;
}
