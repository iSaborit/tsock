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

#include "../include/cli.h"

void construire_message(char *message, char motif, int lg);

void afficher_message(char *message, int lg);

void todo() {
    printf("todo!");
    exit(1);
}

int main(int argc, char **argv) {
    Parameters params;
    if (parse(argc, argv, &params) == -1) {
        perror("parse");
        exit(1);
    }
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
                construire_message(message, (char)i, params.largeur_message);
                afficher_message(message, params.largeur_message);
                int r_sendto;
                if ((r_sendto = sendto(sock, message, strlen(message), 0,
                                       (struct sockaddr *)&addr_distant,
                                       sizeof(addr_distant))) < 30) {
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

            if (bind(sock, (struct sockaddr *)&addr_local, sizeof(addr_local)) <
                0) {
                perror("bind");
                exit(1);
            }

            printf("Listening on the port %d\n", params.port);

            struct sockaddr_in addr_sender;
            socklen_t addr_sender_len = sizeof(addr_sender);

            if (params.nb_message == -1)
                params.nb_message = 100000;
            for (int i = 97; i < params.nb_message + 97; i++) {
                char message[params.largeur_message + 1];
                int r_recvfrom;
                if ((r_recvfrom = recvfrom(
                         sock, message, params.largeur_message, 0,
                         (struct sockaddr *)&addr_sender, &addr_sender_len)) !=
                    params.largeur_message) {
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
