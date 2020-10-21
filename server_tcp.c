#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <netinet/ip.h>

#include "common.h"

int server_run(uint16_t port) {
    int sockfd = tcp_create_socket();

    struct sockaddr_in sockaddr = {0};
    init_addr(&sockaddr, port);

    // Bind to port
    if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_in)) == -1) {
        perror("failed to bind socket to address");
        tcp_destroy_socket(sockfd);
        return -1;
    }

    // Prepare to handle at most n clients
    if (listen(sockfd, 1) == -1) {
        perror("failed to listen for connections");
        tcp_destroy_socket(sockfd);
        return -1;
    }

    printf("Server started on port %d\n", port);

    char buf[4096];

    int stop = 0;
    while (!stop) {
        int client_sockfd;
        struct sockaddr_in client_sockaddr;
        socklen_t client_sockaddr_length = sizeof(client_sockaddr);

        client_sockfd = accept(
            sockfd,
            (struct sockaddr*)&client_sockaddr,
            &client_sockaddr_length
        );

        if (client_sockfd == -1) {
            perror("failed to accept connection from client");
            tcp_destroy_socket(sockfd);
            return -1;
        }

        ssize_t data_size = recv(client_sockfd, buf, sizeof(buf), 0);
        if (data_size == -1) {
            perror("error reading incoming data");
            close(client_sockfd);
            tcp_destroy_socket(sockfd);
            return -1;
        }

        struct header hdr = {0};
        ssize_t payload_size = unwrap(&hdr, buf, sizeof(buf));
        if (payload_size == -1) {
            fprintf(stderr, "failed to process incoming message -> ignoring\n");
            close(client_sockfd);
            continue;
        }

        log_recv(&hdr, buf);

        if (strncmp(buf, "stop", 4) == 0) stop = 1;

        log_send(buf);

        data_size = wrap(buf, sizeof(buf));
        if (data_size == -1) {
            fprintf(stderr, "failed to create response packet");
            close(client_sockfd);
            continue;
        }

        data_size = send(client_sockfd, buf, data_size, 0);
        if (data_size == -1) {
            perror("failed to send response");
            close(client_sockfd);
            continue;
        }

        close(client_sockfd);
    }

    printf("Shutting down...\n");

    tcp_destroy_socket(sockfd);
    return 0;
}