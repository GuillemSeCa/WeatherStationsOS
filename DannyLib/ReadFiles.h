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
#include "Config.h"

//Estructura per guardar les dades d'una estació
typedef struct{
    char * data;
    char * hora;
    float temperatura;
    int humitat;
    float pressioAtmosferica;
    float precipitacio;
}Estacio;

//Mètode per llegir la carpeta i tots els fitxers del seu interior
void readDirectori(DIR * directori);

//Mètode per llegir un fitxer de text, retornant quan és el final del fitxer
char* read_until_end(int fd, char end, int * fiFitxer);

//Mètode per llegir la informació d'una estació, en un fitxer
void readEstacio(Estacio * estacio, char * path);

//Mètode per llegir un fitxer de text utilitzant FD fins a cert caràcter
char* read_until(int fd, char end);