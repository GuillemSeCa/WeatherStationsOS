//#define _GNU_SOURCE
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include "ConfigJack.h"
#include "SignalsJack.h"
#include "SocketsJack.h"
#include "semaphore_v2.h"

//Constants
#define MAX_CONNECTIONS 20
#define MSG_BENVINGUDA "\nStarting Jack...\n\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"

//Variables globals
int memCompId;
ConfigJack configJack;
semaphore jackSem, lloydSem;
Station *stationShared;
Hallorann estacio;

//Mètode que executarà el Fork
void lloydProcess() {
    //Inicialitzem el valor de l'estació
    estacio.countLectures = 0;
    estacio.totalHumidity = 0;
    estacio.totalTemperature = 0.0f;
    estacio.totalAtmospheric = 0.0f;
    estacio.totalPrecipitation = 0.0f;

    //Reprogramem la signal Alarm per a que cada 2 minuts es reescrigui el fitxer de "Hallorann.txt"
    signal(SIGALRM, writeFile);
    //TODO: Canviar a 120 (cada 2 minuts)
    alarm(10);
    signal(SIGINT, ctrlCSignalLloyd);

    //Anem esperant a que s'escrigui a la memòria compartida
    while(1) {
        SEM_wait(&jackSem);
        estacio.countLectures++;
        estacio.totalTemperature += stationShared->temperature;
        estacio.totalHumidity += stationShared->humidity;
        estacio.totalAtmospheric += stationShared->atmosphericPressure;
        estacio.totalPrecipitation += stationShared->precipitation;
        SEM_signal(&lloydSem);
    }
}

int main(int argc, char **argv) {
    pid_t pid;

    //Comprovem que el número d'arguments sigui correcte
    if (argc != 2) {
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

    //Canviem el que fa per defecte el CTRL+C
    signal(SIGINT, ctrlCSignal);
    signal(SIGKILL, ctrlCSignal);

    //Iniciem el semaphore
    SEM_constructor_with_name(&jackSem, ftok("JackLib/Jack.c", 'a'));
    SEM_constructor_with_name(&lloydSem, ftok("JackLib/Jack.c", 'b'));
    //Iniciem el semaphore
    SEM_init(&jackSem, 0);
    SEM_init(&lloydSem, 1);

    //Creem regió memòria compartida
    memCompId = shmget(IPC_PRIVATE, sizeof(Station), IPC_CREAT|IPC_EXCL|0600);
    if (memCompId > 0) {
        //Lliguem les adreces i obtenim el punter de la memòria
        stationShared = shmat(memCompId, 0, 0);
    } else {
        write(1, "Error al crear la regió de memòria compartida!\n", 50);
    }

    //Fork per Lloyd
    pid = fork();
    if (pid == 0) {
        //Child
        lloydProcess();

        //Deslliguem les adreces
        shmdt(stationShared);
    }
    else if (pid > 0) {
        //Parent

        //Missatge benvinguda
        write(1, MSG_BENVINGUDA, strlen(MSG_BENVINGUDA));

        //Llegim el fitxer de configuració per obtenir la IP i Port d'aquest server Jack
        readConfigFileJack(&configJack, argv[1]);

        //Preparem la configuració d'aquest servidor Jack
        launchServer(configJack);
        
        //Iniciem el servidor i esperem connexió dels Clients Danny
        serverRun();

        //Deslliguem les adreces
        shmdt(stationShared);
        //Eliminem la regió compartida
        shmctl(memCompId, IPC_RMID, NULL);
    }

    //Alliberem tota la memòria dinàmica restant i tanquem tot
    closeServer();
    //Deslliguem les adreces
    shmdt(stationShared);
    //Eliminem la regió compartida
    shmctl(memCompId, IPC_RMID, NULL);

    return 0;
}