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

int net_tcp_bind(int port, const int *socket_fd);
int net_tcp_create_sock(int *fd);

int net_tcp_connect(int socket_fd, const char *ip, uint16_t port);
int net_tcp_listen(int socket_fd, int max_number);
int net_tcp_accept(int socket_fd, struct sockaddr *client, socklen_t *client_len, int *socket_fd_bis);
int net_tcp_read(int socket_fd_bis, char * message, int message_length);
int net_tcp_write(int socket_fd_bis, const char * message, int message_length);

#endif // TSOCK_NET_H
