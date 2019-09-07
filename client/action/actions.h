#ifndef BEZPIECZNE_PRZESYLANIE_PLIKOW_ACTIONS_H
#define BEZPIECZNE_PRZESYLANIE_PLIKOW_ACTIONS_H

#include <stdbool.h>
#include "../file/file_utils.h"

/**
 * \file
 * \brief akcje
 *
 * client może wykonywać dwie akcje "downlaod" i "send", tu znajdują się wszystkie metody które dotyczą tych akcji
 */

char *SEND = "send";
char *DOWNLOAD = "download";

struct Action {
    char actionType[50];
    char fileName[200];
};

bool checkActionTypeAndFileName(char *action, char *fileName, struct Action *newAction);

/**
 * pobieramy polecenie z terminala i zwracamy Action - czyli typ akcji i nazwę pliku
 */
struct Action getActionTypeAndFileName(){
    struct Action newAction = {"",""};
    char *buff = NULL;
    size_t buffSize = 180;

    for (;;){
        //pobieramy linijkne od uzytkownika
        buff = (char *)malloc(buffSize * sizeof(char));
        if(buff == NULL)
        {
            perror("Unable to allocate buffer");
            exit(1);
        }
        printf("\nEnter action type \"download\" or \"send\" and file name:\n");
        getline(&buff, &buffSize, stdin);

        char action[50], fileName[200];
        memset(action,'\0',sizeof(action));
        memset(fileName,'\0',sizeof(fileName));
        bool readingFirstWord = false, gotAction = false;
        for (int i=0; i<strlen(buff); i++) {
            char ch = buff[i];
            if (ch != ' ' && ch != '\n') {
                if (!gotAction){
                    snprintf(action + strlen(action), sizeof(action) - strlen(action), "%c", ch);
                    readingFirstWord = true;
                } else {
                    snprintf(fileName + strlen(fileName), sizeof(fileName) - strlen(fileName), "%c", ch);
                }
            } else {
                if (readingFirstWord && !gotAction) {
                    gotAction = true;
                }
            }
        }

//        printf("action: \"%s\" fileName: \"%s\"\n", action, fileName);
        if (checkActionTypeAndFileName(action, fileName, &newAction)){
            strcpy(newAction.actionType, action);
            strcpy(newAction.fileName, fileName);
            return newAction;
        }

    }
}

/**
 * sprawdzamy czy akcja ma dobrą wartość i czy nazwa pliku nie jest pusta
 * @param action
 * @param fileName
 * @param newAction
 */
bool checkActionTypeAndFileName(char *action, char *fileName, struct Action *newAction){
    if ((strcmp(action, SEND) != 0 ) && (strcmp(action, DOWNLOAD) != 0 )) {
        printf("bad action type, action can be \"send\" or \"download\"\n");
        return false;
    }

    if (strcmp(fileName, "") == 0) {
        printf("fileName is nil\n");
        return false;
    }

//    if (strcmp(action, SEND) == 0 ) {
//        if(!fileExists(fileName)) {
//            printf("file \"%s\" not exists or bad file type\n", fileName);
//            return false;
//        }
//    }

//    else {
//        if (isFileNameOK(fileName)) {
//            printf("file name \"%s\" is bad\n", fileName);
//            return false;
//        }
//    }

    strcpy(newAction->actionType, action);
    strcpy(newAction->fileName, fileName);
    return true;
}

/**
 * wysyłanie pliku do serwera
 * @param ssl
 * @param fileName
 */
void sendFileToServer(SSL *ssl, char *fileName){
        char msg[300];
        memset(msg,'\0',sizeof(msg));
        strcpy(msg, "send ");
        strcat(msg, fileName);
        SSL_write(ssl, msg, (int) strlen(msg));

        int wrote = 0, error = 0;
        char * fileBuffToSend = getFileBuffToSend(fileName);
        for(wrote = 0; wrote <strlen(fileBuffToSend); wrote += error){
            printf("sending: %s\n", fileBuffToSend);
            error = SSL_write(ssl, fileBuffToSend+wrote, (int) (strlen(fileBuffToSend) - wrote));
            if(error <= 0)
                break;
        }
}

/**
 * pobieranie pliku z serwera
 * @param ssl
 * @param fileName
 */
void downloadFileFromServer(SSL *ssl, char *fileName){
        printf("sending file name %s to download\n", fileName);
        SSL_write(ssl, fileName, (int) strlen(fileName));

        char *filePath = getDownloadFilePath(fileName);
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

        saveFileFromServer(buffer, file);
}

#endif //BEZPIECZNE_PRZESYLANIE_PLIKOW_ACTIONS_H
