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


typedef struct{
    char * ip;
    int port;
}ConfigJack;

void readConfigFileJack(ConfigJack * config, char * path);
