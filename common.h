#pragma once

#include <stdint.h>

#include <sys/socket.h>
#include <netinet/ip.h>


// We need to communicate the host name to the server, so
// we define the format of a message to be:
//  |HLEN|HVAL|DATA|
// Where:
//  HLEN: unsigned int, the length of HVAL
//  HVAL: the hostname of the client, not null-terminated
//  DATA: data stream, null-terminated

// This is a helper struct to ease message processing
struct header {
    // Hostname length and value
    uint8_t hn_length;
    char hn[255]; // Max size of hostname is 253
};

// Convert a port from an int to a uint16_t. Returns 0 on error
uint16_t convert_port(int port);

// Modifies inplace the buffer, adding the protocol layer
ssize_t wrap(char* buf, size_t buf_size);

// Modifies inplace the buffer, removing the protocol layer
ssize_t unwrap(struct header* hdr, char* buf, size_t buf_size);

// Print received message to console
void log_recv(struct header* hdr, char* msg);

// Print sent message to console
void log_send(char* msg);

// Initialize an address struct
void init_addr(struct sockaddr_in* sa, uint16_t port);

// Handles creation of a UDP socket
int udp_create_socket();
// Handles destruction of a UDP socket
void udp_destroy_socket(int sockfd);

// Handles creation of a TCP socket
int tcp_create_socket();
// Handles destruction of a TCP socket
void tcp_destroy_socket(int sockfd);
