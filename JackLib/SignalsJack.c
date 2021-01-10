#include "SignalsJack.h"

//Variable global
Station *stations;
int fdSocket;
int fdClientDannys[20];
int counterDannys;

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal() {
    int i;

    //Desconnectem Jack i alliberem tota la memòria dinàmica restant
    write(1, "\nDisconnecting Jack...", 23);
    close(fdSocket);
    //Tanquem els File Descriptors dels Clients per tancar la connexió activa
    for(i = 0; i < counterDannys; i++) {
        close(fdClientDannys[i]);
    }
    free(stations);
    stations = NULL;
    raise(SIGINT);
}