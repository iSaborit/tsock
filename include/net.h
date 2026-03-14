#ifndef TSOCK_NET_H
#define TSOCK_NET_H

#include "cli.h"

#include <netinet/in.h>
#include <stddef.h>
#include <sys/socket.h>

int net_build_addr(const char *host, struct sockaddr_in *addr, int port);
int net_udp_bind(int port, int *socket_fd);
int net_udp_create_sock(int *fd);
int net_udp_sendto(int socket_fd, const struct sockaddr *dst, socklen_t dst_len,
                   const void *buf, size_t len);
int net_udp_recvfrom(int socket, void *buf, size_t len, struct sockaddr *dst,
                     socklen_t *dst_t);

#endif // TSOCK_NET_H
