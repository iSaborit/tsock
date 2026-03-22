#include "../include/net.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int net_build_addr(const char *host, struct sockaddr_in *addr, int port) {
    memset(addr, 0, sizeof(*addr));

    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);

    /* Resolve the host name to an IPv4 address before connecting/sending. */
    struct hostent *h = gethostbyname(host);
    if (h == NULL) {
        return -1;
    }

    memcpy(&addr->sin_addr, h->h_addr, h->h_length);

    return 0;
}

int net_udp_create_sock(int *fd) {
    if ((*fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    return 0;
}

int net_udp_sendto(int socket, const struct sockaddr *dst, socklen_t dst_t,
                   const void *buf, size_t len) {
    int sendto_result;
    sendto_result = sendto(socket, buf, len, 0, dst, dst_t);
    if (sendto_result < 0) {
        return -1;
    }

    /* Treat a partial UDP send as an error. */
    if ((size_t)sendto_result != len) {
        errno = EIO;
        return -1;
    }

    return 0;
}

int net_udp_bind(const int port, int *socket_fd) {
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));

    /* Bind the socket on all local interfaces for the requested port. */
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(port);

    return bind(*socket_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
}

int net_udp_recvfrom(int socket, void *buf, size_t len, struct sockaddr *dst,
                     socklen_t *dst_len) {
    int recvfrom_result;
    recvfrom_result = recvfrom(socket, buf, len, 0, dst, dst_len);
    if (recvfrom_result < 0) {
        return -1;
    }

    return recvfrom_result;
}

int net_tcp_create_sock(int *fd) {
    if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    return 0;
}

int net_tcp_bind(const int port, const int *socket_fd) {
    struct sockaddr_in local_addr = {0};

    /* Bind the socket on all local interfaces for the requested port. */
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(port);

    return bind(*socket_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
}

int net_tcp_connect(const int socket_fd, const struct sockaddr_in *addr) {
    if (connect(socket_fd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
        return -1;
    }

    return 0;
}

int net_tcp_listen(int socket_fd, int max_number) {
    return listen(socket_fd, max_number);
}

int net_tcp_accept(int socket_fd, struct sockaddr *client,
                   socklen_t *client_len, int *client_socket_fd) {

    /* `accept` expects the input/output size of the client address buffer. */
    *client_len = sizeof(*client);
    if ((*client_socket_fd = accept(socket_fd, client, client_len)) == -1) {
        return -1;
    }

    return 0;
}

/* A single `read` may return fewer bytes than requested: the caller handles the
 * actual size received. */
int net_tcp_read(const int client_socket_fd, char *message,
                 const int message_length) {
    int bytes_received;
    if ((bytes_received = read(client_socket_fd, message, message_length)) ==
        -1) {
        perror("read");
        return -1;
    }

    return bytes_received;
}

int net_tcp_write(const int client_socket_fd, const char *message,
                  const int message_length) {
    int bytes_sent;
    if ((bytes_sent = write(client_socket_fd, message, message_length)) == -1) {
        perror("write");
        return -1;
    }

    return bytes_sent;
}
