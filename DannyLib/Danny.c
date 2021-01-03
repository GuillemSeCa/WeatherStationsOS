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

//Constants
#define MSG_BENVINGUDA "\nStarting Danny...\n\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"
#define MSG_ERR_SOCKET "Error durant la creacio del socket del Client!\n"
#define MSG_ERR_CONN "Error en la connexió amb el servidor (Client)!\n"

//Variables globals
Config config;
DIR *directori;
Estacio *estacio = NULL;
int fdServer;

//Mètode per eliminar memòria
void freeMemory(Estacio *estacio) {
    free(estacio);
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

//Mètode per connectar-se a un servidor a partir d'una IP i un Port
int connectWithServer(char *ip, int port) {
    struct sockaddr_in s_addr;
    int socket_conn = -1;

    socket_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_conn < 0) {
        write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
    } else {

        memset(&s_addr, 0, sizeof(s_addr));
        s_addr.sin_family = AF_INET;
        s_addr.sin_port = htons(port);
        s_addr.sin_addr.s_addr = inet_addr(ip);
        if (connect(socket_conn, (void *) &s_addr, sizeof(s_addr)) < 0) {
            char buff[128];
            write(1, MSG_ERR_CONN, sizeof(MSG_ERR_CONN));
            int bytes = sprintf(buff, "errno says: %s\n", strerror(errno)); // molt útil
            write(1, buff, bytes);
            close(socket_conn);
            socket_conn = -1;
        }
    }

    return socket_conn;
}

//Mètode per tancar el File Director del Servidor
void closeConnectionServer() {
    close(fdServer);
}

//Mètode principal
int main(int argc, char **argv) {
    //Comprovem que el número d'arguments sigui correcte
    if (argc != 2) {
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

    //Canviem el que fa per defecte el CTRL+C
    signal(SIGINT, ctrlCSignal);

    //Missatge benvinguda
    write(1, MSG_BENVINGUDA, strlen(MSG_BENVINGUDA));

    //Llegim la informació de el fitxer de configuració
    readConfigFile(&config, argv[1]);
    //Ens connectem al servidor Jack
    connectWithServer(config.ipJack, config.portJack);

    //Canviem el que es fa per defecte quan es rep una Alarma
    signal(SIGALRM, alarmaSignal);
    //Iniciem el programa
    alarm(1);

    //Bucle infinit fins que fem CTRL+C
    while (1) pause();

    //Alliberem tota la memòria dinàmica restant i tanquem tot
    freeMemory(estacio);
    closeConnectionServer();

    return 0;
}