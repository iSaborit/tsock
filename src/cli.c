#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../include/cli.h"

int cli_parse(int argc, char **argv, struct tsock_config *cfg) {
    int c;
    extern char *optarg;
    extern int optind;
    int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
    bool is_tcp = true;
    int largeur_message = 30;

    Source source = NONE; 
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

    cfg->is_tcp = is_tcp;
    cfg->source = source;
    cfg->nb_message = nb_message;
    cfg->port = port;
    cfg->largeur_message = largeur_message;

    return 0;
}

void cli_print_info(const struct tsock_config cfg) {
    /*
     * This becomes impossible, as params.source will always be SOURCE or PUITS.
     * therefore, in the v1 we will comment it, then we will delete it.
     *  if (params.source == NONE) {
     *      printf(USAGE);
     *      exit(1);
     *  }
     */

    // printing if source or puits (evident)
    if (cfg.source == SOURCE)
        printf("[tsock] on est dans la source\n");
    else
        printf("[tsock] on est dans le puits\n");

    // print nb_message information
    if (cfg.source == SOURCE)
        printf("[tsock] nb de tampons à envoyer : %d\n", cfg.nb_message);
    else if (cfg.source == PUITS && cfg.nb_message != -1)
        printf("[tsock] nb de tampons à recevoir : %d\n", cfg.nb_message);
    else
        printf("[tsock] nb de tampons à recevoir : infini\n");

    printf("[tsock] port number: %d\n", cfg.port);
}
