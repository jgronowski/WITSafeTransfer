#ifndef BEZPIECZNE_PRZESYLANIE_PLIKOW_REQUEST_HANDLER_H
#define BEZPIECZNE_PRZESYLANIE_PLIKOW_REQUEST_HANDLER_H

#include "../file/file_utils.h"

/**
 * \file
 * \brief przetwarzanie żądania clienta
 *
 * tutaj przetwarzamt żądanie clienta i pobieramy / wysyłamy pliki w zależności od żądania
 */

char *SEND = "send";

void downloadFileFromClient(char * request, SSL *ssl);
void sendFileToClient(char * fileName, SSL *ssl);

/**
 * przetwarzanie żądania od klienta
 * @param request
 * @param ssl
 */
void handleRequest(char * request, SSL *ssl){
    char requestType[4];
    strncpy(requestType,request, 4);

    printf("request: %s\n", request);

    if (strcmp(requestType, SEND) == 0 ){
        return downloadFileFromClient(request, ssl);
    } else {
        return sendFileToClient(request, ssl);
    }
}

/**
 * pobiera plik od klienta
 * @param request
 * @param ssl
 */
void downloadFileFromClient(char * request, SSL *ssl){
    printf("Downloading file from client\n");

    char *filePath = getDownloadFilePath(request);
    FILE * file = openFileWrite(filePath);

    //reading response from server
    char buffer[9000];
    int read = 0, error = 0;
    memset(buffer,'\0',sizeof(buffer));
    for(read = 0; read < sizeof(buffer) + read; sizeof(buffer) - read){
        error = SSL_read(ssl, buffer + read, sizeof(buffer) - read);
        if (error <= 0)
            break;
        printf("recived: %s\n", buffer);
    }

    saveFileFromClient(buffer, file);
}

/**
 * wysyła plik do klienta
 * @param fileName
 * @param ssl
 */
void sendFileToClient(char * fileName, SSL *ssl){
    char * fileBuffToSend = getFileBuffToSend(fileName);

    //sending response from server
    int wrote = 0, error = 0;
    for(wrote = 0; wrote <strlen(fileBuffToSend); wrote += error){
        printf("sending: %s\n", fileBuffToSend);
        error = SSL_write(ssl, fileBuffToSend+wrote, (int) (strlen(fileBuffToSend) - wrote));
        if(error <= 0)
            break;
    }
}

#endif //BEZPIECZNE_PRZESYLANIE_PLIKOW_REQUEST_HANDLER_H
