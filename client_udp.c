#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <netinet/ip.h>

#include "common.h"

int client_run(uint16_t port, const char* message) {
    int sockfd = udp_create_socket();

    // Setup server address
    struct sockaddr_in server_sockaddr = {0};
    socklen_t server_sockaddr_len = sizeof(struct sockaddr_in);
    init_addr(&server_sockaddr, port);

    char buf[4096];
    strncpy(buf, message, sizeof(buf));

    log_send(buf);

    ssize_t data_size = wrap(buf, sizeof(buf));
    if (data_size == -1) {
        fprintf(stderr, "failed to create packet");
        udp_destroy_socket(sockfd);
        return -1;
    }

    data_size = sendto(
        sockfd,
        buf,
        data_size,
        0,
        (struct sockaddr*)&server_sockaddr,
        server_sockaddr_len
    );

    if (data_size == -1) {
        perror("failed to send data");
        udp_destroy_socket(sockfd);
        return -1;
    }

    data_size = recvfrom(
        sockfd, 
        buf, 
        sizeof(buf),
        0, 
        (struct sockaddr*)&server_sockaddr_len, 
        &server_sockaddr_len
    );

    if (data_size == -1) {
        perror("error reading response data");
        udp_destroy_socket(sockfd);
        return -1;
    }

    struct header hdr = {0};
    ssize_t payload_size = unwrap(&hdr, buf, sizeof(buf));
    if (payload_size == -1) {
        fprintf(stderr, "failed to process response");
        udp_destroy_socket(sockfd);
        return -1;
    }

    log_recv(&hdr, buf);

    udp_destroy_socket(sockfd);
    return 0;
}