#include <stdlib.h>
#include "tcp_openssl/ssl_client.h"

const int SERVER_PORT = 8080;

int main(int argc, char *argv[]) {
    int port;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <port> \n", argv[0]);
//        exit(1);
        port = SERVER_PORT;
    } else {
        port = atoi(argv[1]);
    }


    startOpenSSLClient(port);

    exit(EXIT_SUCCESS);
}