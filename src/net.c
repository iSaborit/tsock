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

    struct hostent *h = gethostbyname(host);

    memcpy(&addr->sin_addr, h->h_addr, h->h_length);

    return 0;
}

int net_udp_create_sock(int *fd) {
    if ((*fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    return 0;
}

int net_udp_sendto(int socket, const struct sockaddr *dst, socklen_t dst_t, const void *buf, size_t len) {
    int rtn_sendto;
    rtn_sendto = sendto(socket, buf, len, 0, dst, dst_t);
    if (rtn_sendto < 0) {
        return -1;
    }

    if ((size_t)rtn_sendto != len) {
        errno = EIO;
        return -1;
    }

    return 0;
}

int net_udp_bind(const int port, int *socket_fd) {
    struct sockaddr_in addr_local;
    memset(&addr_local, 0, sizeof(addr_local));

    // build client socket address
    addr_local.sin_family = AF_INET;
    addr_local.sin_addr.s_addr = INADDR_ANY;
    addr_local.sin_port = htons(port);

    return bind(*socket_fd, (struct sockaddr *)&addr_local, sizeof(addr_local));
}

int net_udp_recvfrom(int socket, void *buf, size_t len, struct sockaddr *dst, socklen_t *dst_t) {
    int rtn_recvfrom;
    rtn_recvfrom = recvfrom(socket, buf, len, 0, dst, dst_t);
    if (rtn_recvfrom < 0) {
        return -1;
    }

    return rtn_recvfrom;
}

int net_tcp_create_sock(int *fd) {
    if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    return 0;
}

int net_tcp_bind(const int port, const int *socket_fd) {
    struct sockaddr_in addr_local = {0};

    // build client socket address
    addr_local.sin_family = AF_INET;
    addr_local.sin_addr.s_addr = INADDR_ANY;
    addr_local.sin_port = htons(port);

    return bind(*socket_fd, (struct sockaddr *)&addr_local, sizeof(addr_local));
}

int net_tcp_connect(const int socket_fd, const char *ip, uint16_t port) {
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // we experiment with inet_pton because gethostbyname() function is deprecated.
    // still we have used it in this file for net_????????????????????????
    if (inet_pton(AF_INET, ip, &server.sin_addr) != 1) {
        return -1;
    }

    if (connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        return -1;
    }

    return 0;
}

/// Listens.
int net_tcp_listen(int socket_fd, int max_number) {
    return listen(socket_fd, max_number);
}

int net_tcp_accept(int socket_fd, struct sockaddr *client, socklen_t *client_len, int *socket_fd_bis) {
    *client_len = sizeof(*client);
    if ((*socket_fd_bis = accept(socket_fd, client, client_len)) == -1) {
        return -1;
    }

    return 0;
}

int net_tcp_read(const int socket_fd_bis, char * message, const int message_length) {
    int bytes_received;
    if ((bytes_received = read(socket_fd_bis, message, message_length)) == -1) {
        perror("read");
        return -1;
    }

    return bytes_received;
}

int net_tcp_write(const int socket_fd_bis, const char * message, const int message_length) {
    int bytes_sent;
    if ((bytes_sent = write(socket_fd_bis, message, message_length)) == -1) {
        perror("write");
        return -1;
    }

    return bytes_sent;
}
