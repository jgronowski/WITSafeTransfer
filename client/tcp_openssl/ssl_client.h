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
#include <signal.h>
/* SSL */
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "../action/actions.h"
#include "../file/file_utils.h"

/**
 * \file
 * \brief połączenie openssl klienta
 *
 * tu jest tworzone połączenie openssl z serwererm gdzie określamy metode kryptograficzną taką samą jak na serwerze
 */

void startOpenSSLClient(int port);
SSL_CTX* createSSLClientMethodAndCtx(const SSL_METHOD *sslMethod);
void startClient(struct sockaddr_in serverAddress, SSL_CTX *sslCtx);

/**
 * utworzenie openssl klienta
 * @param port domyślnie 8080
 */
void startOpenSSLClient(int port){
    const SSL_METHOD *sslMethod = NULL;
    SSL_CTX *sslCtx = createSSLClientMethodAndCtx(sslMethod);

    //creating server address
    struct sockaddr_in serverAddress;
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons((unsigned short) port);

    startClient(serverAddress, sslCtx);
}

/**
 * tworzenie SSL_METHOD i SSL_CTX
 * @param sslMethod
 * @param sslCtx
 */
SSL_CTX *createSSLClientMethodAndCtx(const SSL_METHOD *sslMethod) {
    SSL_CTX* sslCtx = NULL;

    OpenSSL_add_all_algorithms();
    SSL_library_init();
    SSL_load_error_strings();

    //creating ssl server method
    //SSL_METHOD pozwala okreslic metode kryptograficzna wykorzystywana do komunikacji
    //tutaj zostala uzyta metoda TLS
    sslMethod = TLS_client_method();

    //creating ssl context
    //SSL_CTX określa kontekst komunikacji serwera
    //tutaj przekazujemy SSL_METHOD gdzie uzywamy TLS
    if((sslCtx = SSL_CTX_new(sslMethod)) == NULL){
        ERR_print_errors_fp(stderr);
        exit(-1);
    }
    return sslCtx;
}

/**
 * czekamy na polecenie od użytkownika i wysyłam do serwera żądanie
 * @param clientSocket
 * @param serverAddress
 */
void startClient(struct sockaddr_in serverAddress, SSL_CTX *sslCtx){
    printf("OPEN SSL client started\n");

    for(;;){
        SSL *ssl;

        struct Action action = getActionTypeAndFileName();

        //creating client socket and connecting with server
        int clientSocket = socket(PF_INET, SOCK_STREAM, 0);
        bind(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
        connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

//        if(bind(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
//            printf("error while binding server\n");
//            exit(-1);
//        }
//        if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
//            printf("error while connecting with server\n");
//            exit(-1);
//        }

        //creating ssl and connect
        //SSL to struktura zarzadzajaca danymi niezbednymi do korzystana z beziecznego polaczenia
        if((ssl = SSL_new(sslCtx)) == NULL){
            ERR_print_errors_fp(stderr);
            exit(-1);
        }

        SSL_set_fd(ssl, clientSocket);
        if(SSL_connect(ssl) <= 0){
            ERR_print_errors_fp(stderr);
            exit(-1);
        }

        printf("Connection made with [version,cipher]:[%s,%s]\n", SSL_get_version(ssl), SSL_get_cipher(ssl));

        if (strcmp(action.actionType, SEND) == 0){
            sendFileToServer(ssl, action.fileName);
        } else if (strcmp(action.actionType, DOWNLOAD) == 0) {
            downloadFileFromServer(ssl,action.fileName);
        } else {
            printf("bad action type %s\n", action.fileName);
        }

        //cleaning
        SSL_shutdown(ssl);
        SSL_free(ssl);
//        close(clientSocket);
    }
    SSL_CTX_free(sslCtx);
}

#endif //BEZPIECZNE_PRZESYLANIE_PLIKOW_TCP_CONNECTION_H
