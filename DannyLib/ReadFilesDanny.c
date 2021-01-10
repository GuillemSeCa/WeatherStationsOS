#include "ReadFilesDanny.h"

//Variable global
Config config;
int fdServer;

//Mètode per llegir un fitxer de text utilitzant FD fins a cert caràcter
char *readUntil(int fd, char end) {
    int i = 0, size;
    char c = '\0';
    char *string = (char *) malloc(sizeof(char));
    while (1) {
        size = read(fd, &c, sizeof(char));

        if (c != end && size > 0 && c != '&') {
            string = (char *) realloc(string, sizeof(char) * (i + 2));
            string[i++] = c;
        } else {
            //Pel \0
            i++;
            break;
        }
    }
    string[i - 1] = '\0';
    return string;
}

//Mètode per llegir un fitxer de text, retornant quan és el final del fitxer
char *readUntilEnd(int fd, char end, int *endFile) {
    int i = 0, size;
    char c = '\0';
    char *string = (char *) malloc(sizeof(char));
    while (1) {
        size = read(fd, &c, sizeof(char));

        if (c != end && size > 0 && c != '&') {
            string = (char *) realloc(string, sizeof(char) * (i + 2));
            string[i++] = c;
        } else {
            //Pel \0
            i++;
            if (size == 0) {
                *endFile = 1;
            }
            break;
        }
    }
    string[i - 1] = '\0';
    return string;
}

//Mètode per llegir la informació d'una estació, que es troba en un fitxer .txt
void readEstation(Station *station, char *path, int numStation) {
    int fdStation;

    //Obtenim el file descriptor del fitxer de configuració
    fdStation = open(path, O_RDONLY);

    //Verifiquem que s'hagi obtingut correctament
    if (fdStation < 0) {
        write(1, "Error lectura de fitxer Station!\n", 34);
        //Si s'ha obtingut correctament
    } else {
        //Llegim la informació de l'estació
        station[numStation].date = readUntil(fdStation, '\n');
        station[numStation].hour = readUntil(fdStation, '\n');
        station[numStation].temperature = atof(readUntil(fdStation, '\n'));
        station[numStation].humidity = atoi(readUntil(fdStation, '\n'));
        station[numStation].atmosphericPressure = atof(readUntil(fdStation, '\n'));
        station[numStation].precipitation = atof(readUntil(fdStation, '\n'));
    }

    //Tanquem el file descriptor
    close(fdStation);
}

//Mètode per enviar la informació de les estacions al servidor Jack
void sendStationsToServer(Station *stations, int numStations) {
    int numSend, i=0;

    //Enviem el número d'estacions al servidor
    write(fdServer, &numStations, sizeof(int));

    //Iterem totes les estacions i anem enviant la informació
    write(1, "\nSending data...\n", 17);

    printf("STATION NAME SEND DATA: %s\n", stations[i].fileName);
    for(i = 0; i < numStations; i++) {
        //fileName
        numSend = strlen(stations[i].fileName);
        write(fdServer, &numSend, sizeof(int));
        write(fdServer, stations[i].fileName, sizeof(char) * strlen(stations[0].fileName));
        //date
        numSend = strlen(stations[i].date);
        write(fdServer, &numSend, sizeof(int));
        write(fdServer, stations[i].date, sizeof(char) * strlen(stations[0].date));
        //hour
        numSend = strlen(stations[i].hour);
        write(fdServer, &numSend, sizeof(int));
        write(fdServer, stations[i].hour, sizeof(char) * strlen(stations[0].hour));
        //temperature
        write(fdServer, &stations[i].temperature, sizeof(float));
        //humidity
        write(fdServer, &stations[i].humidity, sizeof(int));
        //atmosphericPressure
        write(fdServer, &stations[i].atmosphericPressure, sizeof(float));
        //precipitation
        write(fdServer, &stations[i].precipitation, sizeof(float));
    }
    write(1, "Data sent\n", 11);
}

//Mètode per llegir la carpeta i tots els fitxers del seu interior
void readDirectory(DIR *directory) {
    struct dirent *entry;
    int countFiles = 0, i = 0;
    char *pathFolder;
    Station *stations = NULL;
    char numTextFilesFound[255], textFilePath[255], aux[255];
    textFilePath[0] = '\0';

    //Obrim el directori
    pathFolder = (char *) malloc(sizeof(char) * (strlen(config.pathFolder) + 1));
    strcat(pathFolder, ".");
    strcat(pathFolder, config.pathFolder);
    directory = opendir(pathFolder);

    //Guardem memòria per les stations i comprovem que s'hagi fet correctament
    stations = (Station *) malloc(sizeof(Station) * 1);
    if (stations == NULL) {
        write(1, "Error reserva memoria stations!\n", 34);
        return;
    }

    //Bucle per recòrrer tots els fitxers de la carpeta
    while ((entry = readdir(directory)) != NULL) {
        countFiles++;
        //Ens saltem els fitxers '.' i '..' que sempre trobarem
        if (countFiles > 2) {
            //Si és un arxiu .txt, llegim la informació del fitxer dades
            if (strstr(entry->d_name, ".txt")) {
                //Augmentem memòria dinàmica de la llista d'stations 
                stations = (Station *) realloc(stations, sizeof(Station) * (countFiles - 1));

                //Guardem el nom del fitxer
                stations[countFiles - 3].fileName = (char *) malloc(sizeof(char) * strlen(entry->d_name));
                strcat(stations[countFiles - 3].fileName, entry->d_name);

                //Guardem el path del fitxer a llegir
                strcat(textFilePath, config.pathFolder);
                strcat(textFilePath, "/");
                strcat(textFilePath, entry->d_name);
                //Eliminem el primer caràcter '/' del path, ja que aquest es troba en el fitxer de configuració però no el volem
                memmove(textFilePath, textFilePath + 1, strlen(textFilePath));

                //Llegim la informació de l'estació
                readEstation(stations, textFilePath, countFiles - 3);

                //Eliminem el fitxer
                //remove(textFilePath);

                //Reiniciem el path al fitxer .txt
                textFilePath[0] = '\0';
            }
        }
    }

    //Eliminem el comptador dels dos fitxers '.' i '..'
    countFiles = countFiles - 2;
    //Si no hi ha fitxers dins de la carpeta
    if (countFiles == 0) {
        write(1, "No files available\n", 20);
        //Si hi ha fitxers dins de la carpeta
    } else {
        //Mostrem tots els fitxers llegits amb la informació corresponent
        sprintf(numTextFilesFound, "%d files found\n", countFiles);
        write(1, numTextFilesFound, strlen(numTextFilesFound));
        for (i = 0; i < countFiles; i++) {
            write(1, stations[i].fileName, strlen(stations[i].fileName));
            write(1, "\n", 1);
        }
        for (i = 0; i < countFiles; i++) {
            sprintf(aux, "\n%s\n", stations[i].fileName);
            write(1, aux, strlen(aux));
            sprintf(aux, "%s\n", stations[i].date);
            write(1, aux, strlen(aux));
            sprintf(aux, "%s\n", stations[i].hour);
            write(1, aux, strlen(aux));
            sprintf(aux, "%.1f\n", stations[i].temperature);
            write(1, aux, strlen(aux));
            sprintf(aux, "%d\n", stations[i].humidity);
            write(1, aux, strlen(aux));
            sprintf(aux, "%.1f\n", stations[i].atmosphericPressure);
            write(1, aux, strlen(aux));
            sprintf(aux, "%.1f\n", stations[i].precipitation);
            write(1, aux, strlen(aux));
        }
    }
    //Enviem la informació al servidor
    sendStationsToServer(stations, countFiles);
    
    //Alliberem tota la memòria dinàmica
    free(pathFolder);
    free(stations);
    stations = NULL;
    closedir(directory);
}