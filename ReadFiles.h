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

//Estructura per guardar les dades d'una estació
typedef struct{
    char * data;
    char * hora;
    float temperatura;
    int humitat;
    float pressioAtmosferica;
    float precipitacio;
}Estacio;

void readDirectori(DIR * directori);

char* read_until_end(int fd, char end, int * fiFitxer);

void readEstacio(Estacio * estacio, char * path);

char* read_until(int fd, char end);
