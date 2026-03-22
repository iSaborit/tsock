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
    int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut :
                            10 en émission, infini en réception */
    bool is_tcp = true;
    int largeur_message = 30;

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
            nb_message = atoi(optarg);
            break;

        case 'l':
            largeur_message = atoi(optarg);
            break;

        case 'u':
            is_tcp = false;
            break;

        default:
            fputs(USAGE, stderr);
            return -1;
        }
    }
    if (source == NONE)
        source = SOURCE;

    if (nb_message == -1 && source == SOURCE)
        nb_message = 10;

    int remaining_args = argc - optind;

    if (remaining_args == 2 && source == PUITS) {
        printf("this is the problemna");
        fputs(USAGE, stderr);
        return -1;
    }

    if (remaining_args == 2 && source == SOURCE) {
        char *dest = argv[optind];
        cfg->dest = malloc(strlen(dest) + 20);
        if (cfg->dest == NULL) {
            perror("malloc");
            return -1;
        }
        sprintf(cfg->dest, "%s.insa-toulouse.fr", dest);
        optind++;
    } else if (remaining_args == 1) {
        cfg->dest = strdup("127.0.0.1");
    } else {
        fputs(USAGE, stderr);
        return -1;
    }

    int port = atoi(argv[optind]);
    if (port == 0) {
        fputs(USAGE, stderr);
        return -1;
    }

    cfg->is_tcp = is_tcp;
    cfg->mode = source;
    cfg->nb_message = nb_message;
    cfg->port = port;
    cfg->message_length = largeur_message;

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
        printf("Number of messages to send: %d, ", cfg.nb_message);
    } else if (cfg.mode == PUITS && cfg.nb_message != -1)
        printf("Number of messages to receive: %d, ", cfg.nb_message);
    else
        printf("Number of messages to receive: infinite.");

    printf("Message length: %d.\n", cfg.message_length);
}
