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



#include "Config.h"
#include "ReadFiles.h"
#include "Signals.h"

//Constants
#define MSG_BENVINGUDA "\nStarting Danny...\n\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"

#define MSG_ERR_SOCKET      "[Cliente] Error durante la creacion del socket.\n"
#define MSG_ERR_CONN        "[Cliente] Error de connexion con el servidor.\n"


//Variables globals
Config config;
DIR * directori;
Estacio * estacio = NULL;
int server_fd;
//Mètode per eliminar memòria
void freeMemoria(Estacio * estacio){
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

int connect_to_server(char *ip, int port) {
    struct sockaddr_in s_addr;
    int socket_conn = -1;

    socket_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     
    if (socket_conn < 0) {
        write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
    } else {
        
        memset(&s_addr, 0, sizeof(s_addr));
        s_addr.sin_family   = AF_INET;
        s_addr.sin_port     = htons(port);
        s_addr.sin_addr.s_addr = inet_addr(ip);
        printf("Fins aqui\n");
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

//Mètode principal
int main(int argc, char ** argv){   
    //Comprovem que el número d'arguments sigui correcte
    if(argc != 2){
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

    //Canviem el que fa per defecte el CTRL+C
    signal(SIGINT, ctrlCSignal);

    //Missatge benvinguda
    write(1, MSG_BENVINGUDA, strlen(MSG_BENVINGUDA));

    connect_to_server(config.ipJack, config.portJack);
    printf("Fins aqui\n");
    //Llegim la informació de el fitxer de configuració
    readConfigFile(&config, argv[1]);
    
    //Canviem el que es fa per defecte quan es rep una Alarma
    signal(SIGALRM, alarmaSignal);
    //Iniciem el programa
    alarm(1);

    //Bucle infinit fins que fem CTRL+C
    while(1) pause();

    //Alliberem tota la memòria dinàmica restant
    freeMemoria(estacio);
    

    return 0;
}