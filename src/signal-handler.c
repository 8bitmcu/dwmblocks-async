#include "signal-handler.h"

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <unistd.h>

#include "block.h"
#include "main.h"
#include "timer.h"

typedef struct signalfd_siginfo signal_info;

signal_handler signal_handler_new(
    block **blocks, const unsigned short block_count,
    const signal_refresh_callback refresh_callback,
    const signal_timer_callback timer_callback) {
    signal_handler handler = {
        .refresh_callback = refresh_callback,
        .timer_callback = timer_callback,

        .blocks = blocks,
        .block_count = block_count,
    };

    return handler;
}

int signal_handler_init(signal_handler *const handler) {
    signal_set set;
    (void)sigemptyset(&set);

    // Handle user-generated signal for refreshing the status.
    (void)sigaddset(&set, REFRESH_SIGNAL);

    // Handle SIGALRM generated by the timer.
    (void)sigaddset(&set, TIMER_SIGNAL);

    // Handle termination signals.
    (void)sigaddset(&set, SIGINT);
    (void)sigaddset(&set, SIGTERM);

    for (unsigned short i = 0; i < handler->block_count; ++i) {
        block *block = handler->blocks[i];
        if (block->signal > 0) {
            if (sigaddset(&set, SIGRTMIN + block->signal) != 0) {
                (void)fprintf(
                    stderr,
                    "error: invalid or unsupported signal specified for "
                    "\"%s\" block\n",
                    block->command);
                return 1;
            }
        }
    }

    // Create a signal file descriptor for epoll to watch.
    handler->fd = signalfd(-1, &set, 0);
    if (handler->fd == -1) {
        (void)fprintf(stderr,
                      "error: could not create file descriptor for signals\n");
        return 1;
    }

    // Block all realtime and handled signals.
    for (int i = SIGRTMIN; i <= SIGRTMAX; ++i) {
        (void)sigaddset(&set, i);
    }
    (void)sigprocmask(SIG_BLOCK, &set, NULL);

    return 0;
}

int signal_handler_deinit(signal_handler *const handler) {
    if (close(handler->fd) != 0) {
        (void)fprintf(stderr,
                      "error: could not close signal file descriptor\n");
        return 1;
    }

    return 0;
}

int signal_handler_process(signal_handler *const handler, timer *const timer) {
    signal_info info;
    const ssize_t bytes_read = read(handler->fd, &info, sizeof(info));
    if (bytes_read == -1) {
        (void)fprintf(stderr, "error: could not read info of incoming signal");
        return 1;
    }

    const int signal = (int)info.ssi_signo;
    switch (signal) {
        case TIMER_SIGNAL:
            if (handler->timer_callback(handler->blocks, handler->block_count,
                                        timer) != 0) {
                return 1;
            }
            return 0;
        case REFRESH_SIGNAL:
            if (handler->refresh_callback(handler->blocks,
                                          handler->block_count) != 0) {
                return 1;
            }
            return 0;
        case SIGTERM:
            // fall through
        case SIGINT:
            return 1;
    }

    for (unsigned short i = 0; i < handler->block_count; ++i) {
        block *const block = handler->blocks[i];
        if (block->signal == signal - SIGRTMIN) {
            const uint8_t button = (uint8_t)info.ssi_int;
            block_execute(block, button);
            break;
        }
    }

    return 0;
}
