#include "../include/app.h"
#include "../include/net.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int run_udp_sender(const struct tsock_config cfg);
static int run_udp_receiver(struct tsock_config cfg);

static int run_tcp_sender(const struct tsock_config cfg);
static int run_tcp_receiver(struct tsock_config cfg);

static void build_message(char *message, const char fill_char, const int lg,
                          const int message_number);
static void print_message(const char *message, const int lg,
                          const int message_number, const Mode mode);

/* Dispatches execution to the TCP/UDP  sender/receiver selected in `cfg`. */
int app_run(const struct tsock_config cfg) {
    if (cfg.is_tcp) {
        if (cfg.mode == SOURCE) {
            if (run_tcp_sender(cfg)) {
                exit(1);
            }
        } else {
            run_tcp_receiver(cfg);
        }
    } else {
        if (cfg.mode == SOURCE) {
            if (run_udp_sender(cfg)) {
                exit(1);
            }
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
    struct sockaddr_in destination_addr;
    if (net_build_addr(cfg.dest, &destination_addr, cfg.port)) {
        fprintf(stderr, "invalid destination: %s:%d ", cfg.dest, cfg.port);
        return -1;
    }

    for (int i = 0; i < cfg.message_count; i++) {
        char message[cfg.message_length + 1];
        build_message(message, (char)(i % 26 + 97), cfg.message_length, i + 1);
        print_message(message, cfg.message_length, i + 1, cfg.mode);

        if (net_udp_sendto(sock, (struct sockaddr *)&destination_addr,
                           sizeof(destination_addr), message,
                           (size_t)cfg.message_length)) {
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
    socklen_t sender_addr_len = sizeof(sender);

    /* In PUITS mode, `-1` means "receive indefinitely"; use INT_MAX as a loop
     * bound. */
    if (cfg.message_count == -1)
        cfg.message_count = INT_MAX;

    for (int i = 0; i < cfg.message_count; i++) {
        char message[cfg.message_length + 1];
        memset(message, '\0', cfg.message_length);

        int recvfrom_result;
        if ((recvfrom_result = net_udp_recvfrom(
                 sock, message, (size_t)cfg.message_length,
                 (struct sockaddr *)&sender, &sender_addr_len)) < 0) {
            perror("recvfrom");
            printf("[tsock] recvfrom return value: %d\n", recvfrom_result);
            continue;
        }
        print_message(message, cfg.message_length, i + 1, cfg.mode);
    }

    close(sock);
    return 0;
}

static int run_tcp_sender(const struct tsock_config cfg) {
    int sock;

    int connected = -1;
    int attempts = 0;
    const int max_attempts = 5;

    /* Retry the TCP connection a few times so the sender can start before the
     * receiver is ready. */
    while (connected == -1 && attempts < max_attempts) {
        if (net_tcp_create_sock(&sock)) {
            perror("socket");
            return -1;
        }

        struct sockaddr_in destination_addr;
        if (net_build_addr(cfg.dest, &destination_addr, cfg.port)) {
            fprintf(stderr, "invalid host: %s", cfg.dest);
            return -1;
        }

        if ((connected = net_tcp_connect(sock, &destination_addr)) == -1) {
            perror("connect");
            close(sock);
            attempts++;
            sleep(1);
        }
    }

    if (connected == -1) {
        fprintf(stderr, "unable to connect after %d attempts\n", max_attempts);
        return -1;
    }

    for (int i = 0; i < cfg.message_count; i++) {
        char message[cfg.message_length + 1];
        build_message(message, (char)(i % 26 + 97), cfg.message_length, i + 1);
        print_message(message, cfg.message_length, i + 1, cfg.mode);

        int bytes_sent;
        if ((bytes_sent = net_tcp_write(sock, message, cfg.message_length)) !=
            cfg.message_length) {
            perror("write");
            printf("[tsock] write return value: %d\n", bytes_sent);
        }
    }

    printf("[SOURCE] End of emission.\n");

    close(sock);

    return 0;
}

static int run_tcp_receiver(struct tsock_config cfg) {
    int sock;

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

    /* In PUITS mode, `-1` means "receive indefinetely"; use INT_MAX as a loop
     * bound.*/
    if (cfg.message_count == -1)
        cfg.message_count = INT_MAX;

    int total_received = 0;
    const int max_messages = cfg.message_count;

    /* Accept successive TCP clients until the requested total number of
     * messages has been received. */
    while (total_received < max_messages) {
        int client_sock;
        struct sockaddr client;
        socklen_t client_len = sizeof(client);

        if (net_tcp_accept(sock, &client, &client_len, &client_sock) == -1) {
            perror("accept");
            continue;
        }

        /* Read messages from the current client until it closes the connection
         * or the global target is reached. */
        while (total_received < max_messages) {
            char message[cfg.message_length + 1];
            memset(message, '\0', cfg.message_length);

            int bytes_received =
                net_tcp_read(client_sock, message, cfg.message_length);

            if (bytes_received == 0) {
                printf("[PUITS] Connection ended by the sender.\n");
                break;
            }

            if (bytes_received < 0) {
                perror("read");
                printf("[tsock] read return value: %d\n", bytes_received);
                continue;
            }
            print_message(message, bytes_received, ++total_received, cfg.mode);
        }
        close(client_sock);
    }

    close(sock);
    return 0;
}

static void build_message(char *message, const char fill_char, const int lg,
                          const int message_number) {

    /* `6` = 5 visible characters for the message id + trailing '\0' for
     * snprintf.*/
    snprintf(message, 6, "%5d", message_number);

    for (int i = 5; i < lg; i++) {
        message[i] = fill_char;
    }

    message[lg] = '\0';
}

static void print_message(const char *message, const int lg,
                          const int message_number, const Mode mode) {
    if (mode == SOURCE) {
        printf("[SOURCE] Message #%d: (%d bytes) <", message_number, lg);
    } else {
        printf("[PUITS] Message #%d: (%d bytes) <", message_number, lg);
    }

    for (int i = 0; i < lg; i++)
        printf("%c", message[i]);

    printf(">\n");
}
