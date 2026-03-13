/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
/* pour le type bool */
#include <stdbool.h>
#include <string.h>

// TODO: change help message
const char message_error[] = "usage: cmd [-p|-s] [-n ##] PORT\n";

typedef enum {
    PUITS,
    SOURCE,
    NONE,
} Source;

// command line interface parameters
typedef struct {
    int nb_message;
    bool is_tcp;
    Source source;
    int largeur_message;

    // Human readable!
    int port;
} CliParams;

CliParams parse(int argc, char **argv);

void print_cli_info(CliParams);

void construire_message(char *message, char motif, int lg);

void afficher_message(char *message, int lg);

void todo() {
    printf("todo!");
    exit(1);
}

int main(int argc, char **argv) {
    CliParams params = parse(argc, argv);
    print_cli_info(params);

    int sock;

    if (params.is_tcp) {
        todo();
    } else {
        if (params.source == SOURCE) {
            struct sockaddr_in addr_distant;
            if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("socket");
                exit(1);
            }
            memset(&addr_distant, 0, sizeof(addr_distant));

            // build socket address destination
            addr_distant.sin_family = AF_INET;
            addr_distant.sin_port = htons(params.port);
            struct hostent *host = gethostbyname("127.0.0.1");
            memcpy(&addr_distant.sin_addr, host->h_addr, host->h_length);

            for (int i = 97; i < params.nb_message + 97; i++) {
                char message[params.largeur_message + 1];
                construire_message(message, (char) i, params.largeur_message);
                afficher_message(message, params.largeur_message);
                int r_sendto;
                if ((r_sendto = sendto(sock, message, strlen(message), 0, (struct sockaddr *) &addr_distant,
                                       sizeof(addr_distant))) <
                    30) {
                    perror("sendto");
                    printf("[tsock] sendto return value: %d\n", r_sendto);
                }
            }
        } else {
            struct sockaddr_in addr_local;
            if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("socket");
                exit(1);
            }
            memset(&addr_local, 0, sizeof(addr_local));

            // build client socket address
            addr_local.sin_family = AF_INET;
            addr_local.sin_addr.s_addr = INADDR_ANY;
            addr_local.sin_port = htons(params.port);

            if (bind(sock, (struct sockaddr *) &addr_local, sizeof(addr_local)) < 0) {
                perror("bind");
                exit(1);
            }

            printf("Listening on the port %d\n", params.port);

            struct sockaddr_in addr_sender;
            socklen_t addr_sender_len = sizeof(addr_sender);

            if (params.nb_message == -1)
                params.nb_message = 100000;
            for (int i = 97; i < params.nb_message + 97; i++) {
                char message[params.largeur_message+1];
                int r_recvfrom;
                if ((r_recvfrom = recvfrom(sock, message, params.largeur_message, 0, (struct sockaddr *) &addr_sender, &addr_sender_len)) != params.largeur_message) {
                    perror("recvfrom");
                    printf("[tsock] recvfrom return value: %d\n", r_recvfrom);
                }
                printf("%.*s\n", 30, message);
            }
        }
    }

    close(sock);

    return 0;
}

CliParams parse(int argc, char **argv) {
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
                    printf("usage: cmd [-p|-s] [-n ##]\n");
                    exit(1);
                }
                source = PUITS;
                break;

            case 's':
                if (source == PUITS) {
                    printf("usage: cmd [-p|-s] [-n ##]\n");
                    exit(1);
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
                printf(message_error);
                break;
        }
    }
    if (source == NONE) source = SOURCE;

    if (nb_message == -1 && source == SOURCE)
        nb_message = 10;

    // Checking if port number was specified
    if (optind >= argc) {
        printf(message_error);
        exit(1);
    }

    int port = atoi(argv[argc - 1]);
    if (port == 0) {
        printf(message_error);
        exit(1);
    }

    CliParams params;
    params.is_tcp = is_tcp;
    params.source = source;
    params.nb_message = nb_message;
    params.port = port;
    params.largeur_message = largeur_message;

    return params;
}

void print_cli_info(CliParams params) {
    /*
     * This becomes impossible, as params.source will always be SOURCE or PUITS.
     * therefore, in the v1 we will comment it, then we will delete it.
     *  if (params.source == NONE) {
     *      printf(message_error);
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

void construire_message(char *message, char motif, int lg) {
    for (int i = 0; i < lg; i++) {
        message[i] = motif;
    }
    for (int j = 0; j < 5; j++) {
        message[j] = '-';
    }
    message[lg] = '\0';
}

void afficher_message(char *message, int lg) {
    printf("message construit: ");

    for (int i = 0; i < lg; i++)
        printf("%c", message[i]);

    printf("\n");
}
