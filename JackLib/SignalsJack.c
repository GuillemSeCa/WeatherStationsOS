#include "SignalsJack.h"

int countClients, memCompId, *clientPIDs;
semaphore jackSem, lloydSem;
Station *stationShared;
Hallorann estacio;

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal() {
    int i;

    //Eliminem memòria compartida i semàfors
    shmdt(stationShared);
    shmctl(memCompId, IPC_RMID, NULL);
    SEM_destructor(&jackSem);
    SEM_destructor(&lloydSem);
    
    //Desconnectem tots els Danny's
    for(i = 0; i < countClients; i++) {
        kill (clientPIDs[i], SIGINT);
    }

    //Desconnectem Jack i alliberem tota la memòria dinàmica restant
    write(1, "\nDisconnecting Jack...\n", 24);
    closeServer();
    raise(SIGINT);
}

//Mètode per substituir el funcionament del signal CTRL+C per Lloyd
void ctrlCSignalLloyd() {
    int i;
    
    //Desconnectem tots els Danny's
    for(i = 0; i < countClients; i++) {
        kill (clientPIDs[i], SIGINT);
    }

    //Desconnectem Lloyd i alliberem tota la memòria dinàmica restant
    closeServer();
    raise(SIGINT);
}

//Mètode per escriure al fitxer Hallorann
void writeFile() {
    int fdHallorann, mitjanaH;
    float mitjanaT, mitjanaA, mitjanaP;
    char buffer[255];
    buffer[0] = '\0';

    write(1, HALLORANN_WRITING_MSG, sizeof(HALLORANN_WRITING_MSG));

    //Eliminem l'anterior fitxer i creem un nou en cas d'existir
    remove(PATH_HALLORANN);
    fdHallorann = open(PATH_HALLORANN, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    //Guardem les dades al fitxer
    mitjanaH = estacio.totalHumidity / estacio.countLectures;
    mitjanaT = estacio.totalTemperature / estacio.countLectures;
    mitjanaA = estacio.totalAtmospheric / estacio.countLectures;
    mitjanaP = estacio.totalPrecipitation / estacio.countLectures;
    sprintf(buffer, "Nombre total lectures: %d\nMitjana humitat: %d\nMitjana temperatura: %.1f\nMitjana pressio: %.1f\nMitjana precipitacio: %.1f\n", estacio.countLectures, mitjanaH, mitjanaT, mitjanaA, mitjanaP);
    buffer[strlen(buffer) - 1] = '\0';
    write(fdHallorann, buffer, strlen(buffer));

    close(fdHallorann);
    //Tornem a reprogramar la signal
    signal(SIGALRM, writeFile); 
    alarm(120);
}