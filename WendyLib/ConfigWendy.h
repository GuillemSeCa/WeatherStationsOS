/*
* Mòdul amb els mètodes per llegir el fitxer de configuració de Wendy
*/
#pragma once

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

//Estructura per guardar la configuració
typedef struct
{
    char *ip;
    int port;
} ConfigWendy;

//Mètode per llegir un fitxer de text utilitzant FD fins a cert caràcter
char *readUntil(int fd, char end);

//Mètode per llegir el fitxer de configuració
void readConfigFileWendy(ConfigWendy *config, char *path);