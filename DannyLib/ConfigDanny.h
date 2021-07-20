/*
* Mòdul amb els mètodes per llegir el fitxer de configuració de Danny
*/
#pragma once

#include "ReadFilesDanny.h"
#include "SocketsDanny.h"

//Estructura per guardar la configuració
typedef struct
{
    char *stationName;
    char *pathFolder;
    int revisionFilesTime;
    char *ipJack;
    int portJack;
    char *ipWendy;
    int portWendy;
} Config;

//Mètode per llegir el fitxer de configuració
void readConfigFile(Config *config, char *path);