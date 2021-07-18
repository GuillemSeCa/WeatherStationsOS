/*
* Mòdul amb els mètodes que treballen amb Signals
*/
#pragma once

#define _POSIX_SOURCE
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
#include <sys/shm.h>
#include "SocketsJack.h"

//Constants
#define HALLORANN_WRITING_MSG "Writing data at file 'Hallorann.txt'\n"
#define PATH_HALLORANN "Hallorann.txt"

//Estructura per guardar la mitjana aritmètica
typedef struct {
    int countLectures;
    int totalHumidity;
    float totalTemperature;
    float totalAtmospheric;
    float totalPrecipitation;
} Hallorann;

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal();

//Mètode per substituir el funcionament del signal CTRL+C per Lloyd
void ctrlCSignalLloyd();

//Mètode per escriure al fitxer Hallorann
void writeFile();