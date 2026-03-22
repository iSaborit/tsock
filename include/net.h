#ifndef TSOCK_NET_H
#define TSOCK_NET_H

#include <netinet/in.h>
#include <stddef.h>
#include <sys/socket.h>

/**
 * Builds an IPv4 socket address from a host and a port.
 *
 * Returns `0` on success and `-1` if the host cannot be resolved.
 */
int net_build_addr(const char *host, struct sockaddr_in *addr, int port);

/* Creates an UDP socket and stores its file descriptor in `fd`. */
int net_udp_create_sock(int *fd);

/* Binds an existing UDP socket to the given local port. */
int net_udp_bind(int port, int *socket_fd);

/**
 * Sends exactly `len` bytes througth UDP.
 *
 * Returns `0` on success and `-1` on error or partial send.
 */
int net_udp_sendto(int socket_fd, const struct sockaddr *dst, socklen_t dst_len,
                   const void *buf, size_t len);

/**
 * Recieves up to `len` bytes through UDP, returning the numbers of bytes read.
 *
 * Note:
 * - If the sent message was greater than `len`, the message will be read
 * partially, so the data will be lost.
 * ˆˆˆˆˆˆ
 * This is a feature, not a bug.
 * (It really is a feature, it's done thinking in the TP we've done in the first
 * semester, this can demonstrate how easily is to lose data from UDP, and that
 * UDP is a Datagram protocol.)
 */
int net_udp_recvfrom(int socket, void *buf, size_t len, struct sockaddr *dst,
                     socklen_t *dst_len);

/* Creates a TCP socket and stores its file descriptor in `fd`. */
int net_tcp_create_sock(int *fd);

/* Binds an existing TCP socket to the given local port. */
int net_tcp_bind(int port, const int *socket_fd);

/* Connects a TCP socket to a remote IPv4 address. */
int net_tcp_connect(int socket_fd, const struct sockaddr_in *addr);

/* Marks a TCP socket as passive with the given backlog: `max_number`. */
int net_tcp_listen(int socket_fd, int max_number);

/* Accepts an incoming TCP connection and stores the new socket in
 * `client_socket_fd` */
int net_tcp_accept(int socket_fd, struct sockaddr *client,
                   socklen_t *client_len, int *client_socket_fd);

/**
 * Reads from a TCP socket.
 *
 * Returns the number of bytes read, up to `message_length`; `0` if the peer
 * closed the connection or `-1` on error.
 *
 * Note:
 * - We intentionally accept that a message may be fragmented or combined upon
 * reception. This highlights an important property: TCP is stream-oriented, not
 * datagram-oriented.
 *
 * It's a feature, not a bug... Again. :)
 */
int net_tcp_read(int client_socket_fd, char *message, int message_length);

/**
 * Writes to a TCP socket.
 *
 * Returns the number of bytes written or `-1` on error.
 */
int net_tcp_write(int client_socket_fd, const char *message,
                  int message_length);

#endif // TSOCK_NET_H
