/*
* Mòdul amb els mètodes per llegir el fitxer de configuració de Wendy
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
} ConfigWendy;

//Mètode per llegir un fitxer de text utilitzant FD fins a cert caràcter
char *readUntil(int fd, char end);

//Mètode per llegir el fitxer de configuració
void readConfigFileWendy(ConfigWendy *config, char *path);