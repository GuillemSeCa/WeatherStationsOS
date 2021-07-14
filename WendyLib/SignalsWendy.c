#include "SignalsWendy.h"

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal() {
    //Desconnectem Wendy i alliberem tota la memòria dinàmica restant
    write(1, "\nDisconnecting Wendy...\n", 25);
    closeServer();
    raise(SIGINT);
}