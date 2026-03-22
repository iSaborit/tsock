#ifndef TSOCK_CLI_H
#define TSOCK_CLI_H

#define USAGE                                                                  \
    "Usage: tsock [-p | -s] [-n <nb_messages>] [-l <message_length>] [-u] [dest] "    \
    "<port>\n"

#include <stdbool.h>

typedef enum {
    PUITS,
    SOURCE,
    NONE,
} Mode;

struct tsock_config {
    int nb_message;
    bool is_tcp;
    Mode mode;
    int message_length;

    // Human readable!
    int port;
    char *dest;
} ;

int cli_parse(int argc, char **argv, struct tsock_config *cfg);
void cli_print_info(struct tsock_config params);

#endif // TSOCK_CLI_H
