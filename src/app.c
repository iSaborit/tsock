#include "../include/app.h"
#include "../include/net.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int todo(void) {
    fprintf(stderr, "Nuh uh.\n");
    return -1;
}

static int run_udp_sender(const struct tsock_config cfg);
static int run_udp_receiver(struct tsock_config cfg);

static int run_tcp_sender(const struct tsock_config cfg);
static int run_tcp_receiver(struct tsock_config cfg);

static void build_message(char *message, const char motif, const int lg);
static void print_message(const char *message, const int lg);

int app_run(const struct tsock_config cfg) {
    if (cfg.is_tcp) {
        if (cfg.source == SOURCE) 
            run_tcp_sender(cfg);
        else 
            run_tcp_receiver(cfg);

    } else {
        if (cfg.source == SOURCE) {
            run_udp_sender(cfg);
        } else {
            run_udp_receiver(cfg);
        }
    }
    return 0;
}

static int run_udp_sender(struct tsock_config cfg) {
    int sock;
    if (net_udp_create_sock(&sock)) {
        perror("socket");
        exit(1);
    }

    // build socket address destination
    struct sockaddr_in destination;
    if (net_build_addr("127.0.0.1", &destination, cfg.port)) {
        perror("bind");
        exit(1);
    }

    for (int i = 97; i < cfg.nb_message + 97; i++) {
        char message[cfg.largeur_message + 1];
        build_message(message, (char)i, cfg.largeur_message);
        print_message(message, cfg.largeur_message);

        if (net_udp_sendto(sock, (struct sockaddr *)&destination,
                           sizeof(destination), message,
                           cfg.largeur_message)) {
            fprintf(stderr, "Problem: %d", errno);
        }
    }

    return 0;
}

static int run_udp_receiver(struct tsock_config cfg) {
    int sock;
    net_udp_bind(cfg.port, &sock);
    printf("Listening on the port %d\n", cfg.port);

    struct sockaddr_in addr_sender;
    socklen_t addr_sender_len = sizeof(addr_sender);

    if (cfg.nb_message == -1)
        cfg.nb_message = 100000;

    for (int i = 97; i < cfg.nb_message + 97; i++) {
        char message[cfg.largeur_message + 1];
        int r_recvfrom;
        if ((r_recvfrom = net_udp_recvfrom(sock, message, cfg.largeur_message,
                                           (struct sockaddr *)&addr_sender,
                                           &addr_sender_len)) !=
            cfg.largeur_message) {
            perror("recvfrom");
            printf("[tsock] recvfrom return value: %d\n", r_recvfrom);
        }
        print_message(message, cfg.largeur_message);
    }

    return 0;
}

static int run_tcp_sender(const struct tsock_config cfg) {
    return todo();
}

static int run_tcp_receiver(struct tsock_config cfg) {
    return todo();
}

static void build_message(char *message, const char motif, const int lg) {
    for (int i = 0; i < lg; i++) {
        message[i] = motif;
    }
    for (int j = 0; j < 5; j++) {
        message[j] = '-';
    }
    message[lg] = '\0';
}

static void print_message(const char *message, const int lg) {
    printf("message construit: ");

    for (int i = 0; i < lg; i++)
        printf("%c", message[i]);

    printf("\n");
}
