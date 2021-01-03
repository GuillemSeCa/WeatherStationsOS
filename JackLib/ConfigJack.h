/*
* Mòdul amb els mètodes per llegir el fitxer de configuració de Jack
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

//Estructura per guardar la configuració
typedef struct {
    char *ip;
    int port;
} ConfigJack;

//Mètode per llegir un fitxer de text utilitzant FD fins a cert caràcter
char *readUntil(int fd, char end);

//Mètode per llegir el fitxer de configuració
void readConfigFileJack(ConfigJack *config, char *path);