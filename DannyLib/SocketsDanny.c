#include "SocketsDanny.h"

//Variables globals
int fdServer, fdServerWendy;
Config config;
Station *stations;

//Mètode per connectar-se a un servidor a partir d'una IP i un Port
int connectWithServer(char *ip, int port) {
    struct sockaddr_in socketAdress;
    int fdSocket = -1;
    char buff[128];

    //Obtenim el File Descriptor del Socket (sense connexió)
    fdSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Comprovem que s'hagi obtingut correctament
    if (fdSocket < 0) {
        write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
    } else {
        //Carguem l'adreça amb les dades corresponents (IP, Port, etc.)
        memset(&socketAdress, 0, sizeof(socketAdress));
        socketAdress.sin_family = AF_INET;
        socketAdress.sin_port = htons(port);
        socketAdress.sin_addr.s_addr = inet_addr(ip);

        //Ens connectem al servidor a partir de l'adreça i File Descriptor del Socket
        if (connect(fdSocket, (void *) &socketAdress, sizeof(socketAdress)) < 0) {
            //En cas d'error al connectar-nos amb el servidor, mostrem missatge informatiu i tanquem tot
            write(1, MSG_ERR_CONNECTION, sizeof(MSG_ERR_CONNECTION));
            int bytes = sprintf(buff, "Error errno: %s\n", strerror(errno));
            write(1, buff, bytes);
            close(fdSocket);
            fdSocket = -1;
        }
    }

    return fdSocket;
}

//Mètode per tancar els File Descriptors dels Servidors
void closeConnectionServer() {
    close(fdServer);
    close(fdServerWendy);
}

//Mètode per alliberar la memòria dinàmica
void freeMemory() {
    free(stations);
    stations = NULL;
    free(config.stationName);
    config.stationName = NULL;
    free(config.pathFolder);
    config.pathFolder = NULL;
    free(config.ipJack);
    config.ipJack = NULL;
    free(config.ipWendy);
    config.ipWendy = NULL;
}