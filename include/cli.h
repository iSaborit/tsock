#ifndef TSOCK_CLI_H
#define TSOCK_CLI_H

// command line interface parameters

// TODO: change help message
const char message_error[] = "usage: cmd [-p|-s] [-n ##] PORT\n";

typedef enum {
    PUITS,
    SOURCE,
    NONE,
} Source;

typedef struct {
    int nb_message;
    bool is_tcp;
    Source source;
    int largeur_message;

    // Human readable!
    int port;
} Parameters;

int parse(int argc, char **argv, Parameters *params);

void print_cli_info(const Parameters params);

#endif //TSOCK_CLI_H