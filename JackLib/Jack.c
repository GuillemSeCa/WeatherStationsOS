#include <stdio.h>
#include <sys/socket.h>

#include "ReadConfigJack.h"

#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"

#define MSG_ERR_SOCKET      "[Servidor] Error durante la creacion del socket.\n"

//int danny_fd;

//ConfigJack configJack;

int main(int argc, char ** argv){   
    //struct sockaddr_in s_addr;
    printf("Hola\n");
    //Comprovem que el número d'arguments sigui correcte
    if(argc != 2){
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

    //readConfigFileJack(configJack, argv[1]);
    //printf("IP: %s - PORT: %d \n", configJack.ip, configJack.port);

    /*
    *danny_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (*socket_fd < 0) {
        write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
        return -1;
    }

    bzero(&s_addr, sizeof (s_addr));
    */

}