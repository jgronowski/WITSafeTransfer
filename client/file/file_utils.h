#ifndef BEZPIECZNE_PRZESYLANIE_PLIKOW_FILE_TRANSPORT_H
#define BEZPIECZNE_PRZESYLANIE_PLIKOW_FILE_TRANSPORT_H

#include <limits.h>

/**
 * \file
 * \brief zarządzanie plikami
 *
 * wszystkie metody tutaj pomagają zarządzać plikami
 */

char* getResourcesFilePath(char *fileName);

/**
 * zwraca ścieżkę do pliku z folderu /downloads
 * @param fileName
 */
char* getDownloadFilePath(char *fileName){
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
    strcat(newFilePath,"/client");

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

/**
 * sprawdza czy plik istnieje w /resources
 * @param fileName
 */
bool fileExists(char *fileName) {
    char * filePath = getResourcesFilePath(fileName);
    FILE *fp;
    fp = fopen(filePath, "w");

    if (fp == NULL)
    {
        return false;
    }

    fclose(fp);
    return true;
}

/**
 * sprawdza czy plik istnieje w /downloads
 * @param fileName
 */
bool isFileNameOK(char *fileName) {
    char * filePath = getDownloadFilePath(fileName);
    FILE *fp;
    fp = fopen(filePath, "w");

    if (fp == NULL)
    {
        return false;
    }

    fclose(fp);
    return true;
}

//sending
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
    strcat(filePath,"/client");

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


#endif //BEZPIECZNE_PRZESYLANIE_PLIKOW_FILE_TRANSPORT_H
