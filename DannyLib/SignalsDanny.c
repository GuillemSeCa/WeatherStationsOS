#include "SignalsDanny.h"

//Variables globals
Config config;
DIR *directory;

//Mètode per substituir el funcionament del signal Alarma
void alarmaSignal() {
    //Mostrem missatge nom estacio per pantalla
    write(1, "\n", 1);
    write(1, "$", 1);
    write(1, config.stationName, strlen(config.stationName));
    write(1, ":\n", 3);
    //Llegeix els directoris
    write(1, "Testing...\n", 12);
    readDirectory(directory);

    //Reiniciem alarma
    signal(SIGALRM, alarmaSignal);
    alarm(config.revisionFilesTime);
}

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal() {
    //TODO: Arreglar
    //Mostrem missatge nom estacio per pantalla
    /*write(1, "\n$", 2);
    write(1, config.stationName, strlen(config.stationName));
    write(1, ":", 1);

    //Desconnectem Danny, la connexió amb Jack i alliberem tota la memòria dinàmica restant
    write(1, "\nDisconnecting Jack...", 23);
    close(fdServer);
    
    free(stations);
    stations = NULL;*/
    //Desconnectem Danny i alliberem tota la memòria dinàmica restant
    write(1, "\nDisconnecting Danny...", 24);
    closeConnectionServer();
    freeMemory();
    raise(SIGINT);
}