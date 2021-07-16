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
#define MSG_CONNECTING_JACK "Connecting Jack...\n"
#define MSG_CONNECTING_WENDY "Connecting Wendy...\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"

//Variables globals
int fdServer, fdServerWendy;
Config config;

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
    Packet paquet;

    //Comprovem que el número d'arguments sigui correcte
    if (argc != 2) {
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

    //Canviem el que fa per defecte el CTRL+C
    signal(SIGINT, ctrlCSignal);
    //Canviem el que es fa per defecte quan es rep una Alarma
    signal(SIGALRM, alarmaSignal);

    //Missatge benvinguda
    write(1, MSG_WELCOME, strlen(MSG_WELCOME));

    //Llegim la informació de el fitxer de configuració
    readConfigFile(&config, argv[1]);
    
    //Ens connectem al servidor Jack
    write(1, MSG_CONNECTING_JACK, strlen(MSG_CONNECTING_JACK));
    fdServer = connectWithServer(config.ipJack, config.portJack);
    //Enviem primer paquet per verificar la connexió
    strcpy(paquet.origen, "DANNY"); 
	paquet.origen[5] = '\0';
    paquet.tipus = 'C';
	strcpy(paquet.dades, config.stationName);
	write(fdServer, &paquet, sizeof(Packet));
    
    //Escoltem resposta per saber si la connexió ha sigut correcte
    read(fdServer, &paquet, sizeof(Packet));
    if(paquet.tipus == 'O' && strcmp(paquet.origen, "JACK") == 0 && strcmp(paquet.dades, "CONNEXIO OK") == 0) {
        //Ens connectem al servidor Wendy
        write(1, MSG_CONNECTING_WENDY, strlen(MSG_CONNECTING_WENDY));
        fdServerWendy = connectWithServer(config.ipWendy, config.portWendy);
        //Enviem primer paquet per verificar la connexió
        strcpy(paquet.origen, "DANNY"); 
        paquet.origen[5] = '\0';
        paquet.tipus = 'C';
        strcpy(paquet.dades, config.stationName);
        write(fdServerWendy, &paquet, sizeof(Packet));

        //Escoltem resposta per saber si la connexió ha sigut correcte
        read(fdServerWendy, &paquet, sizeof(Packet));
        if(paquet.tipus == 'O' && strcmp(paquet.origen, "WENDY") == 0 && strcmp(paquet.dades, "CONNEXIO OK") == 0) {
            //Iniciem el programa
            alarm(1);
            //Bucle infinit fins que fem CTRL+C per anar llegint les dades i enviant-les
            while (1) {
                pause();
            }
        } else {
            write(1, "Error al connectar amb el servidor Wendy\n", 42);
        }
    } else {
        write(1, "Error al connectar amb el servidor Jack\n", 41);
    }

    //Alliberem tota la memòria dinàmica restant i tanquem tot
    closeConnectionServer();
    freeMemory();

    return 0;
}