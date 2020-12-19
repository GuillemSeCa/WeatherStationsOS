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

#include "Config.h"
#include "ReadFiles.h"

#define MSG_BENVINGUDA "\n\nStarting Danny...\n\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"

//Variables globals
Config config;
DIR * directori;
Estacio * estacio = NULL;

//Mètode per eliminar memòria
void freeMemoria(Estacio * estacio){
    free(estacio);
}

//Mètode per eliminar un caràcter
void removeChar(char *str, char garbage) {
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

//Mètode per substituir el funcionament del signal Alarma
void alarmaSignal(){
    //Mostrem missatge nom estacio per pantalla
    write(1, "$", 1);
    write(1, config.nomEstacio, strlen(config.nomEstacio));
    write(1, ":\n", 3);
    //Llegeix els directoris
    write(1, "Testing...\n", 12);
    readDirectori(directori);

    signal(SIGALRM, alarmaSignal);
    alarm(config.tempsRevisioFixers);
}

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal(){
    //Llegeix els directoris
    write(1, "\nDisconnecting Danny...", 23);
    raise(SIGINT);
}

int main(int argc, char ** argv){   
    //Comprovem que el número d'arguments sigui correcte
    if(argc != 2){
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

    //Canviem el que fa per defecte el CTRL+C
    signal(SIGINT, ctrlCSignal);

    //Missatge benvinguda
    write(1, MSG_BENVINGUDA, strlen(MSG_BENVINGUDA));

    //Llegim la informació de el fitxer de configuració
    readConfigFile(&config, argv[1]);
    
    signal(SIGALRM, alarmaSignal);
    alarm(1);

    while(1) pause();

    //Alliberem tota la memòria dinàmica
    freeMemoria(estacio);
}
