#ifndef TSOCK_CLI_H
#define TSOCK_CLI_H

#define USAGE                                                                  \
    "Usage: tsock [-p | -s] [-n <nb_messages>] [-l <message_length>] [-u] "    \
    "<port>\n"

#include <stdbool.h>

typedef enum {
    PUITS,
    SOURCE,
    NONE,
} Source;

struct tsock_config {
    int nb_message;
    bool is_tcp;
    Source source;
    int largeur_message;

    // Human readable!
    int port;
} ;

int cli_parse(int argc, char **argv, struct tsock_config *cfg);
void cli_print_info(const struct tsock_config params);

#endif // TSOCK_CLI_H
