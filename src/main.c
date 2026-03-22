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
#include "../include/app.h"



int main(int argc, char **argv) {
    struct tsock_config cfg;
    cli_parse(argc, argv, &cfg);
    cli_print_info(cfg);
    app_run(cfg);

    free(cfg.dest);

    return 0;
}