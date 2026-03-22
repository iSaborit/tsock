#ifndef TSOCK_CLI_H
#define TSOCK_CLI_H

#define USAGE                                                                  \
    "Usage: tsock [-p | -s] [-n <message_count>] [-l <message_length>] [-u] "  \
    "[dest] "                                                                  \
    "<port>\n"

#include <stdbool.h>

typedef enum {
    PUITS,
    SOURCE,
    NONE, // Temporary value used during CLI parsing.
} Mode;

struct tsock_config {
    int message_count;  // In PUITS mode, -1 means infinite reception.
    bool is_tcp;
    Mode mode;
    int message_length;

    int port;           // Human readable.
    char *dest;         // Dynamically allocated by cli_parse, freed by caller.
};

/**
 * Parse command-line arguments and fills a `tsock_config` structure.
 *
 * Parameters:
 * - `argc`  : The argument count, provided by `main()` in `main.c`.
 * - `argv`  : The argument values, provided by `main()` in `main.c`.
 * - `cfg`   : Output configuration, filled on success.
 *
 * Returns:
 * - `0`   if everything goes as expected.
 * - `-1`  if it fails.
 *
 * Preconditions:
 * - `cfg` must not be `NULL`.
 * - `argc` and `argv` must be the values received by `main`.
 *
 * Postconditions:
 * - `cfg->dest` is dinamically allocated, must be freed by the caller.
 *
 * Side effects:
 * - Writes usage or error messages to `stderr` on failure.
 *
 * Notes:
 * - Default mode is `SOURCE`.
 * - Default tranport protocol is TCP.
 * - In `SOURCE` mode, the default number of messages is `10`.
 * - In `PUITS` mode, `message_count == -1`
 * - If no destination is provided, `127.0.0.1` is used by default.
 * - `-p`/`-s` are mutually exclusive.
 * - `message_length` must be at least 5.
 * - `port` must be in the range [1, 65535].
 */
int cli_parse(int argc, char **argv, struct tsock_config *cfg);

/* Prints the `tsock_config` structure in a human readable, comfortable way. */
void cli_print_info(struct tsock_config params);

#endif // TSOCK_CLI_H
