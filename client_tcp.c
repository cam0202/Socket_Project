#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>

#include "common.h"

int client_run(uint16_t port, const char* message) {
    int sockfd = tcp_create_socket();

    // Setup server address
    struct sockaddr_in server_sockaddr = {0};
    socklen_t server_sockaddr_len = sizeof(struct sockaddr_in);
    init_addr(&server_sockaddr, port);

    if (connect(
        sockfd,
        (struct sockaddr*)&server_sockaddr, 
        server_sockaddr_len
    ) == -1) {
        perror("failed to connect to server");
        tcp_destroy_socket(sockfd);
        return -1;
    }

    char buf[4096];
    strncpy(buf, message, sizeof(buf));

    log_send(buf);

    ssize_t data_size = wrap(buf, sizeof(buf));
    if (data_size == -1) {
        fprintf(stderr, "failed to create packet");
        close(sockfd);
        tcp_destroy_socket(sockfd);
        return -1;
    }

    data_size = send(sockfd, buf, data_size, 0);
    if (data_size == -1) {
        perror("failed to send data");
        close(sockfd);
        tcp_destroy_socket(sockfd);
        return -1;
    }

    data_size = recv(sockfd, buf, sizeof(buf), 0);
    if (data_size == -1) {
        perror("error reading response data");
        close(sockfd);
        tcp_destroy_socket(sockfd);
        return -1;
    }

    struct header hdr = {0};
    ssize_t payload_size = unwrap(&hdr, buf, sizeof(buf));
    if (payload_size == -1) {
        fprintf(stderr, "failed to process response");
        close(sockfd);
        tcp_destroy_socket(sockfd);
        return -1;
    }

    log_recv(&hdr, buf);

    close(sockfd);

    tcp_destroy_socket(sockfd);
    return 0;
}