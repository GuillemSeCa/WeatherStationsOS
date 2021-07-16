#include "SignalsJack.h"

int countClients, *clientPIDs;

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal() {
    int i;
    
    //Desconnectem tots els Danny's
    for(i = 0; i < countClients; i++) {
        kill (clientPIDs[i], SIGINT);
    }

    //Desconnectem Jack i alliberem tota la memòria dinàmica restant
    write(1, "\nDisconnecting Jack...\n", 24);
    closeServer();
    raise(SIGINT);
}