#ifndef BEZPIECZNE_PRZESYLANIE_PLIKOW_TCP_CONNECTION_H
#define BEZPIECZNE_PRZESYLANIE_PLIKOW_TCP_CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
/* SSL */
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rsa.h>

#include "../file/file_utils.h"
#include "../request/request_handler.h"

/**
 * \file
 * \brief połączenie openssl serwera
 *
 * tu jest tworzony serwer gdzie określamy metode kryptograficzną, dołączamy certyfikat openssl i czekamy na żądania klienta
 */

const int SERVER_PORT = 8080;

void startOpenSSLServer(int port);
SSL_CTX* createSSLServerMethodAndCtx(const SSL_METHOD *sslMethod);
void listenAndServe(int serverSocket, SSL_CTX *sslCtx);

/**
 * utworzenie openssl serwer
 * @param port domyslnie 8080
 */
void startOpenSSLServer(int port){
    const SSL_METHOD *sslMethod = NULL;
    SSL_CTX *sslCtx = createSSLServerMethodAndCtx(sslMethod);

    //creating server socket
    int serverSocket;
    struct sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons((unsigned short) port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    serverSocket = socket(PF_INET, SOCK_STREAM, 0);

    bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    listenAndServe(serverSocket, sslCtx);
}

/**
 * tworzenie SSL_METHOD i SSL_CTX
 * @param sslMethod
 */
SSL_CTX* createSSLServerMethodAndCtx(const SSL_METHOD *sslMethod){
    SSL_CTX *sslCtx = NULL;

    OpenSSL_add_all_algorithms();
    SSL_library_init();
    SSL_load_error_strings();

    //creating ssl server method
    sslMethod = TLS_server_method();

    //creating ssl context
    if((sslCtx = SSL_CTX_new(sslMethod)) == NULL){
        ERR_print_errors_fp(stderr);
        exit(-1);
    }

    //adding mycert.pem file to ssl ctx
    SSL_CTX_use_certificate_file(sslCtx, "/home/jgronowski/Dokumenty/Projekty/Bezpieczne_przesylanie_plikow/server/certificate/mycert.pem", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(sslCtx, "/home/jgronowski/Dokumenty/Projekty/Bezpieczne_przesylanie_plikow/server/certificate/mycert.pem", SSL_FILETYPE_PEM);
    if(!SSL_CTX_check_private_key(sslCtx)){
        fprintf(stderr, "Private key does not match certificate\n");
        exit(-1);
    }

    return sslCtx;
}

/**
 * czekanie na żądania klienta i wysyłanie / pobieranie plików
 * @param serverSocket
 * @param sslCtx
 */
void listenAndServe(int serverSocket, SSL_CTX *sslCtx) {

    printf("OPEN SSL server started\n");

    for (;;){
        SSL *ssl;
        struct sockaddr_in clientAddress;
        int clientSocket, clientSize;

        listen(serverSocket, 5);

        //creating client socket
        clientSize = sizeof(clientAddress);
        bzero(&clientAddress, sizeof(clientAddress));
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, (socklen_t *)&clientSize);
        if((ssl = SSL_new(sslCtx)) == NULL){
            ERR_print_errors_fp(stderr);
            exit(-1);
        }

        //checking ssl certificate
        SSL_set_fd(ssl, clientSocket);
        if(SSL_accept(ssl) <= 0){
            ERR_print_errors_fp(stderr);
            exit(-1);
        }

        printf("\nConnection made with [version,cipher]:[%s,%s]\n",SSL_get_version(ssl),SSL_get_cipher(ssl));

        char request[2000];
        memset(request,'\0',sizeof(request));
        SSL_read(ssl, &request, sizeof(request));

        handleRequest(request, ssl);

        //cleaning
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(clientSocket);
    }
    SSL_CTX_free(sslCtx);
}

#endif //BEZPIECZNE_PRZESYLANIE_PLIKOW_TCP_CONNECTION_H
