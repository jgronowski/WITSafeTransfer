#ifndef BEZPIECZNE_PRZESYLANIE_PLIKOW_FILE_TRANSPORT_H
#define BEZPIECZNE_PRZESYLANIE_PLIKOW_FILE_TRANSPORT_H

#include <limits.h>

/**
 * \file
 * \brief zarządzanie plikami
 *
 * wszystkie metody tutaj pomagają zarządzać plikami
 */

/**
 * zwraca ścieżkę do pliku z folderu /resources
 * @param fileName
 */
char* getResourcesFilePath(char *fileName){
    char filePath[9000];
    memset(filePath,'\0',sizeof(filePath));

    //get current path
    if (getcwd(filePath, sizeof(filePath)) == NULL){
        fprintf(stderr, "Could not get current path!\n");
        exit(1);
    }

//    uncomment if you use CMakeLists

    //delete '/cmake-build-debug' from path
    int currentPathLen = (int) strlen(filePath);
    for(int i=0; i<18; i++){
        filePath[currentPathLen - (1+i)] = '\0';
    }
    strcat(filePath,"/server");
//

    strcat(filePath, "/resources/");
    strcat(filePath, fileName);

    char * path = filePath;
    return path;
}

/**
 * otwiera plik do pisania
 * @param fileName
 */
FILE * openFileRead(char *fileName){
    char* filePath = getResourcesFilePath(fileName);

    printf("Opening file for reading %s\n", filePath);
    FILE * file = fopen(filePath, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open file for reading!\n");
        exit(1);
    }

    return file;
}

/**
 * zwraca plik do wysłania
 * @param fileName
 */
char * getFileBuffToSend(char *fileName){
    int fd;
    char buffer[9000];
    memset(buffer,'\0',sizeof(buffer));

    FILE* file = openFileRead(fileName);
    fd = fileno(file);
    read(fd, buffer, sizeof(buffer));

    char * fileToSend = buffer;
    return fileToSend;
}

//receiving file

/**
 * zwraca nazwę pliku z żądania
 * @param request
 */
char *getFileNameFromRequest(char *request){
    char fileName[200];
    memset(fileName,'\0',sizeof(fileName));
    strcpy(fileName, &request[5]);
    char *fileNameChar = fileName;
    return fileNameChar;
}

/**
 * zwraca ścieżkę do pliku z folderu /downloads
 * @param fileName
 */
char* getDownloadFilePath(char *request){
    char *fileName = getFileNameFromRequest(request);


    //wszystkie operacje ponijzej to jest utworzenie napisu ze sciezka do nowego pliku
    char newFilePath[9000];
    memset(newFilePath,'\0',sizeof(newFilePath));

    //get current path
    if (getcwd(newFilePath, sizeof(newFilePath)) == NULL){
        fprintf(stderr, "Could not get current path!\n");
        exit(1);
    }

//    uncomment if you use CMakeLists

    //delete '/cmake-build-debug' from path
    int currentPathLen = (int) strlen(newFilePath);
    for(int i=0; i<18; i++){
        newFilePath[currentPathLen - (1+i)] = '\0';
    }
    strcat(newFilePath,"/server");
//

    strcat(newFilePath, "/downloads/");
    strcat(newFilePath, fileName);

    char * path = newFilePath;
    return path;

}

/**
 * otwiera plik do czytania
 * @param fileName
 */
FILE * openFileWrite(char *fileName){
    FILE *fp;
    printf("Opening file for writing %s\n", fileName);
    fp = fopen(fileName, "w");

    if (fp == NULL)
    {
        fprintf(stderr, "Could not open destination file\n");
        exit(1);
    }
    return fp;
}

/**
 * zapisuje plik z serwera
 * @param buffer
 * @param file
 */
void saveFileFromServer(char * buffer, FILE * file){
    fputs(buffer, file);
    bzero(buffer, sizeof(buffer));
    fclose(file);
    printf("Downloaded file successful\n");
}

#endif //BEZPIECZNE_PRZESYLANIE_PLIKOW_FILE_TRANSPORT_H
