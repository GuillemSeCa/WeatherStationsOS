#include "SignalsDanny.h"

//Variables globals
Config config;
int fdServer, fdServerWendy;

//Mètode per substituir el funcionament del signal Alarma
void alarmaSignal()
{
    //Mostrem missatge nom estacio per pantalla
    write(1, "\n", 1);
    write(1, "$", 1);
    write(1, config.stationName, strlen(config.stationName));
    write(1, ":\n", 3);
    //Llegeix els directoris
    write(1, "Testing...\n", 12);
    readDirectory();

    //Reiniciem alarma
    signal(SIGALRM, alarmaSignal);
    alarm(config.revisionFilesTime);
}

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal()
{
    Packet paquet;

    //Informem a Jack de la desconnexió
    strcpy(paquet.origen, "DANNY");
    paquet.origen[5] = '\0';
    paquet.tipus = 'Q';
    strcpy(paquet.dades, config.stationName);
    write(fdServer, &paquet, sizeof(Packet));

    //Informem a Wendy de la desconnexió
    strcpy(paquet.origen, "DANNY");
    paquet.origen[5] = '\0';
    paquet.tipus = 'Q';
    strcpy(paquet.dades, config.stationName);
    write(fdServerWendy, &paquet, sizeof(Packet));

    //Desconnectem Danny i alliberem tota la memòria dinàmica restant
    write(1, "\nDisconnecting Danny...", 24);
    closeConnectionServer();
    freeMemory();
    raise(SIGINT);
}