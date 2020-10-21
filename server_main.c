#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "common.h"

extern int server_run(uint16_t port);

static void usage(const char* prog_name) {
    fprintf(stderr, "Usage: %s port\n", prog_name);
}

int main(int argc, char *argv[])
{
    assert(argc > 0);
    if (argc != 2) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    uint16_t port = convert_port(atoi(argv[1]));
    if (port == 0) {
        fprintf(stderr, "invalid argument port %s\n", argv[1]);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    return server_run(port);
}


