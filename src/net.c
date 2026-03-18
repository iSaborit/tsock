#include "../include/net.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int net_udp_create_sock(int *fd) {
    if ((*fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    return 0;
}

int net_udp_sendto(int socket, const struct sockaddr *dst, socklen_t dst_t,
                   const void *buf, size_t len) {
    int rtn_sendto;
    rtn_sendto = sendto(socket, buf, len, 0, dst, dst_t);
    if (rtn_sendto < 0) {
        return -1;
    }

    if ((size_t) rtn_sendto != len) {
        errno = EIO;
        return -1;
    }

    return 0;
}

int net_build_addr(const char *host, struct sockaddr_in *addr, int port) {
    memset(addr, 0, sizeof(*addr));

    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);

    struct hostent *h = gethostbyname(host);

    memcpy(&addr->sin_addr, h->h_addr, h->h_length);

    return 0;
}

int net_udp_bind(int port, int *socket_fd) {
    struct sockaddr_in addr_local;
    if ((*socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    memset(&addr_local, 0, sizeof(addr_local));

    // build client socket address
    addr_local.sin_family = AF_INET;
    addr_local.sin_addr.s_addr = INADDR_ANY;
    addr_local.sin_port = htons(port);

    return bind(*socket_fd, (struct sockaddr *)&addr_local, sizeof(addr_local));
}

int net_udp_recvfrom(int socket, void *buf, size_t len, struct sockaddr *dst,
                     socklen_t *dst_t) {
    int rtn_recvfrom;
    rtn_recvfrom = recvfrom(socket, buf, len, 0, dst, dst_t);
    if (rtn_recvfrom < 0) {
        return -1;
    }

    return rtn_recvfrom;
}

