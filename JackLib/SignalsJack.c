#include "SignalsJack.h"

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal() {
    //Desconnectem Jack i alliberem tota la memòria dinàmica restant
    write(1, "\nDisconnecting Jack...\n", 24);
    closeServer();
    raise(SIGINT);
}