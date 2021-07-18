#include "SignalsJack.h"

int countClients, memCompId, *clientPIDs;
semaphore jackSem, lloydSem;
Station *stationShared;

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