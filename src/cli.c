#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../include/cli.h"

int parse(int argc, char **argv, Parameters *params) {
    int c;
    extern char *optarg;
    extern int optind;
    int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
    bool is_tcp = true;
    int largeur_message = 30;

    Source source = NONE; /* 0=puits, 1=source */
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
                break;
        }
    }
    if (source == NONE) source = SOURCE;

    if (nb_message == -1 && source == SOURCE)
        nb_message = 10;

    // Checking if port number was specified
    if (optind >= argc) {
        fputs(USAGE, stderr);
        return -1;
    }

    int port = atoi(argv[argc - 1]);
    if (port == 0) {
        fputs(USAGE, stderr);
        return -1;
    }

    params->is_tcp = is_tcp;
    params->source = source;
    params->nb_message = nb_message;
    params->port = port;
    params->largeur_message = largeur_message;

    return 0;
}

void print_cli_info(const Parameters params) {
    /*
     * This becomes impossible, as params.source will always be SOURCE or PUITS.
     * therefore, in the v1 we will comment it, then we will delete it.
     *  if (params.source == NONE) {
     *      printf(USAGE);
     *      exit(1);
     *  }
     */

    // printing if source or puits (evident)
    if (params.source == SOURCE)
        printf("[tsock] on est dans la source\n");
    else
        printf("[tsock] on est dans le puits\n");

    // print nb_message information
    if (params.source == SOURCE)
        printf("[tsock] nb de tampons à envoyer : %d\n", params.nb_message);
    else if (params.source == PUITS && params.nb_message != -1)
        printf("[tsock] nb de tampons à recevoir : %d\n", params.nb_message);
    else
        printf("[tsock] nb de tampons à recevoir : infini\n");

    printf("[tsock] port number: %d\n", params.port);
}
