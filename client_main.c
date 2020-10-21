#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "common.h"

extern int client_run(uint16_t port, const char* message);

static void usage(const char* prog_name) {
    fprintf(stderr, "Usage: %s port message\n", prog_name);
}

int main(int argc, char *argv[])
{
    assert(argc > 0);
    if (argc != 3) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    uint16_t port = convert_port(atoi(argv[1]));
    if (port == 0) {
        fprintf(stderr, "invalid argument port %s\n", argv[1]);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    return client_run(port, argv[2]);
}


