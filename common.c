#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

uint16_t convert_port(int port) {
    const uint16_t port_max = ~0;
    if (port <= 0 || port > port_max) {
        return 0;
    }
    return (uint16_t) port;
}

ssize_t wrap(char* buf, size_t buf_size) {
    // Prepare a header struct to ease data processing
    struct header hdr = {0};
    if (gethostname(hdr.hn, sizeof(hdr.hn)) == -1) {
        perror("failed to get host name");
        return -1;
    }

    // gethostname returns a null-terminated string but we don't
    // want the null character in the header
    hdr.hn_length = strnlen(hdr.hn, sizeof(hdr.hn));
    size_t hdr_length = sizeof(hdr.hn_length) + hdr.hn_length;

    // Message will fit in the buffer
    size_t payload_size = strnlen(buf, buf_size) + 1;
    if (hdr_length + payload_size > buf_size) {
        fprintf(stderr, "message too large");
        return -1;
    }

    if (buf[hdr_length + payload_size - 1] != '\0') {
        fprintf(stderr, "message is ill-formed\n");
        return -1;
    }

    // Make room for header stick to the front
    memmove(buf + hdr_length, buf, payload_size);
    memcpy(buf + offsetof(struct header, hn_length), &hdr.hn_length, sizeof(hdr.hn_length));
    memcpy(buf + offsetof(struct header, hn), hdr.hn, hdr.hn_length);

    return hdr_length + payload_size;
}

ssize_t unwrap(struct header* hdr, char* buf, size_t buf_size) {
    // Prepare a header struct to ease data processing
    memcpy(&hdr->hn_length, buf + offsetof(struct header, hn_length), sizeof(hdr->hn_length));

    // The length of the hostname is larger than the message itself
    size_t hdr_length = sizeof(hdr->hn_length) + hdr->hn_length;
    if (hdr_length > buf_size) {
        fprintf(stderr, "message is ill-formed\n");
        return -1;
    }

    memcpy(&hdr->hn, buf + offsetof(struct header, hn), hdr->hn_length);

    // The message is not null-terminated
    size_t payload_size = strnlen(buf + hdr_length, buf_size - hdr_length) + 1;
    if (buf[hdr_length + payload_size - 1] != '\0') {
        fprintf(stderr, "message is ill-formed\n");
        return -1;
    }

    // Move the payload to the front
    memmove(buf, buf + hdr_length, payload_size);

    return payload_size;
}

void log_recv(struct header* hdr, char* msg) {
    printf(
        "Received from %s\n-> %s\n",
        hdr->hn,
        msg
    );
}

void log_send(char* msg) {
    printf(
        "Sending\n-> %s\n",
        msg
    );
}

void init_addr(struct sockaddr_in* sa, uint16_t port) {
    // Prepare the struct, setting address to localhost and port
    memset(sa, 0, sizeof(struct sockaddr_in));
    sa->sin_family = AF_INET;
    sa->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sa->sin_port = htons(port);
}

int udp_create_socket() {
    // Initialize a DGRAM socket = UDP
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("failed to create UDP socket");
    }
    return sockfd;
}

void udp_destroy_socket(int sockfd) {
    // Not much to do here
    shutdown(sockfd, SHUT_RDWR);
}

int tcp_create_socket() {
    // Initialize a DGRAM socket = UDP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("failed to create TDP socket");
    }
    return sockfd;
}

void tcp_destroy_socket(int sockfd) {
    // Not much to do here
    shutdown(sockfd, SHUT_RDWR);
}