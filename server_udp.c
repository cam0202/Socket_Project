#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <netinet/ip.h>

#include "common.h"

int server_run(uint16_t port) {
    int sockfd = udp_create_socket();

    // Setup server address
    struct sockaddr_in sockaddr = {0};
    init_addr(&sockaddr, port);

    // Bind to port
    if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_in)) == -1) {
        perror("failed to bind socket to address");
        udp_destroy_socket(sockfd);
        return -1;
    }

    printf("Server started on port %d\n", port);

    char buf[4096];

    int stop = 0;
    while (!stop) {
        struct sockaddr_in client_sockaddr;
        socklen_t client_sockaddr_length = sizeof(client_sockaddr);

        ssize_t data_size = recvfrom(
            sockfd, 
            buf, 
            sizeof(buf),
            0, 
            (struct sockaddr*)&client_sockaddr, 
            &client_sockaddr_length
        );

        if (data_size == -1) {
            perror("error reading incoming data");
            udp_destroy_socket(sockfd);
            return -1;
        }

        struct header hdr = {0};
        ssize_t payload_size = unwrap(&hdr, buf, sizeof(buf));
        if (payload_size == -1) {
            fprintf(stderr, "failed to process incoming message -> ignoring\n");
            continue;
        }

        log_recv(&hdr, buf);

        if (strncmp(buf, "stop", 4) == 0) stop = 1;

        log_send(buf);
        
        data_size = wrap(buf, sizeof(buf));
        if (data_size == -1) {
            fprintf(stderr, "failed to create response packet");
            continue;
        }

        data_size = sendto(
            sockfd,
            buf,
            data_size,
            0,
            (struct sockaddr*)&client_sockaddr, 
            client_sockaddr_length
        );

        if (data_size == -1) {
            perror("failed to send response");
            continue;
        }
    }

    printf("Shutting down...\n");

    udp_destroy_socket(sockfd);
    return 0;
}