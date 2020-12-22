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
#include "Signals.h"

//Constants
#define MSG_BENVINGUDA "\nStarting Danny...\n\n"
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

//Mètode principal
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
    
    //Canviem el que es fa per defecte quan es rep una Alarma
    signal(SIGALRM, alarmaSignal);
    //Iniciem el programa
    alarm(1);

    //Bucle infinit fins que fem CTRL+C
    while(1) pause();

    //Alliberem tota la memòria dinàmica restant
    freeMemoria(estacio);
}