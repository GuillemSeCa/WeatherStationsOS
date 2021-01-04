/*
* Mòdul amb els mètodes per llegir fitxers en general
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "ConfigDanny.h"

//Estructura per guardar les dades d'una estació
typedef struct {
    char *fileName;
    char *date;
    char *hour;
    float temperature;
    int humidity;
    float atmosphericPressure;
    float precipitation;
} Station;

//Mètode per llegir un fitxer de text utilitzant FD fins a cert caràcter
char *readUntil(int fd, char end);

//Mètode per llegir un fitxer de text fins a cert caràcter, però afegint que retorni si és final del fitxer
char *readUntilEnd(int fd, char end, int *endFile);

//Mètode per llegir la carpeta i tots els fitxers del seu interior
void readDirectory(DIR *directory);

//Mètode per llegir la informació d'una estació, que es troba en un fitxer .txt
void readStation(Station *station, char *path, int numStation);