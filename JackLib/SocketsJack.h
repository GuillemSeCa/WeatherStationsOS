/*
* Mòdul amb els mètodes que treballen amb Sockets
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>

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