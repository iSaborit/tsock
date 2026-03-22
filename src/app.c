#include "../include/app.h"
#include "../include/net.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

/*
static int todo(void) {
    fprintf(stderr, "Nuh uh.\n");
    return -1;
}
*/

static int run_udp_sender(const struct tsock_config cfg);
static int run_udp_receiver(struct tsock_config cfg);

static int run_tcp_sender(const struct tsock_config cfg);
static int run_tcp_receiver(struct tsock_config cfg);

static void build_message(char *message, const char motif, const int lg, const int message_number);
static void print_message(const char *message, const int lg, const int message_number, const Mode mode);

int app_run(const struct tsock_config cfg) {
    if (cfg.is_tcp) {
        if (cfg.mode == SOURCE)
            run_tcp_sender(cfg);
        else
            run_tcp_receiver(cfg);
    } else {
        if (cfg.mode == SOURCE) {
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

    // build destination socket address
    struct sockaddr_in destination;
    net_build_addr("127.0.0.1", &destination, cfg.port);

    for (int i = 0; i < cfg.nb_message; i++) {
        char message[cfg.message_length + 1];
        build_message(message, (char)(i % 26 + 97), cfg.message_length, i + 1);
        print_message(message, cfg.message_length, i, cfg.mode);

        if (net_udp_sendto(sock, (struct sockaddr *)&destination,
                           sizeof(destination), message,
                           cfg.message_length)) {
            fprintf(stderr, "Problem: %d", errno);
        }
    }

    printf("[SOURCE] End of emission.");

    close(sock);
    return 0;
}

static int run_udp_receiver(struct tsock_config cfg) {
    int sock;

    if (net_udp_create_sock(&sock)) {
        perror("socket");
        exit(1);
    }

    if (net_udp_bind(cfg.port, &sock)) {
        perror("bind");
        exit(1);
    }
    printf("Listening on the port %d\n", cfg.port);

    struct sockaddr_in sender;
    socklen_t addr_sender_len = sizeof(sender);

    if (cfg.nb_message == -1)
        cfg.nb_message = INT_MAX-97;

    for (int i = 97; i < cfg.nb_message + 97; i++) {
        char message[cfg.message_length + 1];
        memset(message, '\0', cfg.message_length);

        int r_recvfrom;
        if ((r_recvfrom = net_udp_recvfrom(sock, message, cfg.message_length, (struct sockaddr *)&sender,
                                           &addr_sender_len)) < 0) {
            perror("recvfrom");
            printf("[tsock] recvfrom return value: %d\n", r_recvfrom);
        }
        print_message(message, cfg.message_length, i, cfg.mode);
    }

    close(sock);
    return 0;
}

static int run_tcp_sender(const struct tsock_config cfg) {
    int sock;
    const char *ip = "127.0.0.1";

    if (net_tcp_create_sock(&sock)) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in destination;
    net_build_addr(ip, &destination, cfg.port);

    while (net_tcp_connect(sock, ip, cfg.port) == -1) {
        perror("connect");
    }

    for (int i = 0; i < cfg.nb_message; i++) {
        char message[cfg.message_length + 1];
        build_message(message, (char)(i % 26 + 97), cfg.message_length, i + 1);
        print_message(message, cfg.message_length, i, cfg.mode);

        int bytes_sent;
        if ((bytes_sent = net_tcp_write(sock, message, cfg.message_length)) != cfg.message_length) {
            perror("write");
            printf("[tsock] write return value: %d\n", bytes_sent);
        }
    }

    printf("[SOURCE] End of emission.");

    close(sock);

    return 0;
}

static int run_tcp_receiver(struct tsock_config cfg) {
    int sock, sock_bis;
    struct sockaddr client;
    socklen_t client_len;

    if (net_tcp_create_sock(&sock)) {
        perror("socket");
        exit(1);
    }
    if (net_tcp_bind(cfg.port, &sock)) {
        perror("bind");
        exit(1);
    }
    if (net_tcp_listen(sock, INT_MAX)) {
        perror("listen");
        exit(1);
    }

    printf("Listening on the port %d\n", cfg.port);

    while (net_tcp_accept(sock, &client, &client_len, &sock_bis) == -1) {
    }

    if (cfg.nb_message == -1)
        cfg.nb_message = INT_MAX;

    for (int i = 0; i < cfg.nb_message; i++) {
        char message[cfg.message_length + 1];
        memset(message, '\0', cfg.message_length);

        int bytes_received = net_tcp_read(sock_bis,message,cfg.message_length);

        if (bytes_received == 0) {
            printf("[PUITS] Connection ended by the sender.\n");
            break;
        }

        if (bytes_received < 0) {
            perror("read");
            printf("[tsock] read return value: %d\n", bytes_received);
        }

        print_message(message, cfg.message_length, i, cfg.mode);
    }
    close(sock_bis);
    close(sock);
    return 0;
}

static void build_message(char *message, const char motif, const int lg, const int message_number) {
    snprintf(message, 6, "%5d", message_number);

    for (int i = 5; i < lg; i++) {
        message[i] = motif;
    }

    message[lg] = '\0';
}

static void print_message(const char *message, const int lg, const int message_number, const Mode mode) {
    if (mode == SOURCE) {
        printf("[SOURCE] Message #%d: (%d bytes) <", message_number+1, lg);
    } else {
        printf("[PUITS] Message #%d: (%d bytes) <", message_number+1, lg);
    }

    for (int i = 0; i < lg; i++)
        printf("%c", message[i]);

    printf(">\n");
}
