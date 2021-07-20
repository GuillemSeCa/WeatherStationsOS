/*
* Mòdul amb els mètodes per llegir el fitxer de configuració de Jack
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//Estructura per guardar la configuració
typedef struct
{
    char *ip;
    int port;
} ConfigJack;

//Mètode per llegir un fitxer de text utilitzant FD fins a cert caràcter
char *readUntil(int fd, char end);

//Mètode per llegir el fitxer de configuració
void readConfigFileJack(ConfigJack *config, char *path);