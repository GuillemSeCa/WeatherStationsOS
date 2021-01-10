#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include "ConfigDanny.h"
#include "ReadFilesDanny.h"
#include "SignalsDanny.h"
#include "SocketsDanny.h"

//Constants
#define MSG_WELCOME "\nStarting Danny...\n"
#define MSG_JACK "Connecting Jack...\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"

//Variables globals
Config config;
DIR *directory;
Station *stations = NULL;
int fdServer;

//Mètode per eliminar la memòria dinàmica
void freeMemory() {
    free(stations);
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
int main(int argc, char **argv) {
    int numSend;

    //Comprovem que el número d'arguments sigui correcte
    if (argc != 2) {
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

    //Canviem el que fa per defecte el CTRL+C
    signal(SIGINT, ctrlCSignal);

    //Missatge benvinguda
    write(1, MSG_WELCOME, strlen(MSG_WELCOME));

    //Llegim la informació de el fitxer de configuració
    readConfigFile(&config, argv[1]);
    //Ens connectem al servidor Jack i enviem el nom de l'estació
    write(1, MSG_JACK, strlen(MSG_JACK));
    fdServer = connectWithServer(config.ipJack, config.portJack);
    numSend = strlen(config.stationName);
    write(fdServer, &numSend, sizeof(int));
    write(fdServer, config.stationName, sizeof(char) * strlen(config.stationName));

    //Canviem el que es fa per defecte quan es rep una Alarma
    signal(SIGALRM, alarmaSignal);
    //Iniciem el programa
    alarm(1);
    //Bucle infinit fins que fem CTRL+C
    while (1) {
        pause();
    }

    //Alliberem tota la memòria dinàmica restant i tanquem tot
    freeMemory();
    closeConnectionServer();

    return 0;
}