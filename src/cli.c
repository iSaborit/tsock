#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/cli.h"

#include <string.h>

int cli_parse(int argc, char **argv, struct tsock_config *cfg) {
    int c;
    extern char *optarg;
    extern int optind;
    int message_count = -1;
    bool is_tcp = true;
    int message_width = 30;

    Mode source = NONE;
    while ((c = getopt(argc, argv, "psn:l:u")) != -1) {
        switch (c) {
        case 'p':
            if (source == SOURCE) {
                fputs(USAGE, stderr);
                return -1;
            }
            source = PUITS;
            break;

        case 's':
            if (source == PUITS) {
                fputs(USAGE, stderr);
                return -1;
            }
            source = SOURCE;
            break;

        case 'n':
            message_count = atoi(optarg);
            if (message_count <= 0) {
                fputs("USAGE\n", stderr);
                fputs("Consider a positive number of messages.", stderr);
                return -1;
            }
            break;

        case 'l':
            message_width = atoi(optarg);
            break;

        case 'u':
            is_tcp = false;
            break;

        default:
            fputs(USAGE, stderr);
            return -1;
        }
    }

    /* If neither `-p` nor `-s` is given, defaults to SOURCE. */
    if (source == NONE)
        source = SOURCE;

    /* A source sends 10 messages when `-n` is ommited. */
    if (message_count == -1 && source == SOURCE)
        message_count = 10;

    int remaining_args = argc - optind;

    /**
     * Positional arguments depend on the mode:
     * - SOURCE: `[dest] port`
     * - PUITS: `port`
     */
    if (remaining_args == 2 && source == PUITS) {
        fputs(USAGE, stderr);
        return -1;
    }

    if (remaining_args == 2 && source == SOURCE) {
        char *dest = argv[optind];
        cfg->dest = strdup(dest);
        if (cfg->dest == NULL) {
            perror("malloc");
            return -1;
        }
        optind++;
    } else if (remaining_args == 1) {
        cfg->dest = strdup("127.0.0.1");
        if (cfg->dest == NULL) {
            perror("malloc");
            return -1;
        }
    } else {
        fputs(USAGE, stderr);
        return -1;
    }

    /* Parse and validate the port strictly to reject non numeric or out of
     * range values */
    char *endptr = NULL;
    errno = 0;
    long port_long = strtol(argv[optind], &endptr, 10);
    if (errno != 0 || endptr == argv[optind] || *endptr != '\0' ||
        port_long < 1 || port_long > 65535) {
        free(cfg->dest);
        fputs(USAGE, stderr);
        return -1;
    }
    int port = (int)port_long;

    /* Mesages must be at least 5 bytes long because the prefix stores the
     * message number on 5 characters. */
    if (message_width < 5) {
        free(cfg->dest);
        fputs(USAGE, stderr);
        fputs("Please consider a message length greater than 4", stderr);
        return -1;
    }

    cfg->is_tcp = is_tcp;
    cfg->mode = source;
    cfg->message_count = message_count;
    cfg->port = port;
    cfg->message_length = message_width;

    return 0;
}

void cli_print_info(const struct tsock_config cfg) {

    if (cfg.mode == SOURCE)
        printf("[SOURCE]: ");
    else
        printf("[PUITS]: ");

    if (cfg.is_tcp)
        printf("Transport protocol: TCP, ");
    else
        printf("Transport protocol: UDP, ");

    printf("Port number: %d, ", cfg.port);

    if (cfg.mode == SOURCE) {
        printf("Destination: %s, ", cfg.dest);
        printf("Number of messages to send: %d, ", cfg.message_count);
    } else if (cfg.mode == PUITS && cfg.message_count != -1)
        printf("Number of messages to receive: %d, ", cfg.message_count);
    else
        printf("Number of messages to receive: infinite. ");

    printf("Message length: %d.\n", cfg.message_length);
}
