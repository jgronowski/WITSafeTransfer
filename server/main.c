#include <stdlib.h>
#include "tcp_openssl/ssl_server.h"

int main(int argc, char *argv[]) {
    int port;
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <port> \n", argv[0]);
        port = SERVER_PORT;
    } else {
        port = atoi(argv[1]);
    }


    startOpenSSLServer(port);

    exit(EXIT_SUCCESS);
}