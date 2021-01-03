/*
* Mòdul amb els mètodes per llegir el fitxer de configuració de Danny
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
#include "ReadFilesDanny.h"

//Estructura per guardar la configuració
typedef struct {
    char *nomEstacio;
    char *pathCarpeta;
    int tempsRevisioFixers;
    char *ipJack;
    int portJack;
    char *ipWendy;
    int portWendy;
} Config;

//Mètode per llegir el fitxer de configuració
void readConfigFile(Config *config, char *path);