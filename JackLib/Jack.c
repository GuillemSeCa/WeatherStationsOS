#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include "ReadConfigJack.h"

#define LISTEN_BACKLOG		20

#define MSG_BENVINGUDA "Starting Jack...\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"
#define MSG_ERR_BIND        "[Servidor] Error durante el bind del puerto.\n"

#define MSG_ERR_SOCKET      "[Servidor] Error durante la creacion del socket.\n"

int jack_fd;

ConfigJack configJack;

int launch_server(ConfigJack configJack) {
    struct sockaddr_in s_addr;

    jack_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (jack_fd < 0) {
        write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
        return -1;
    }

    bzero(&s_addr, sizeof (s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons (configJack.port);
    s_addr.sin_addr.s_addr = inet_addr(configJack.ip);
    //printf("%d\n", bind (danny_fd, (void *) &s_addr, sizeof (s_addr)));
    if (bind (jack_fd, (void *) &s_addr, sizeof (s_addr)) < 0) {
        write(1, MSG_ERR_BIND, sizeof(MSG_ERR_BIND));
        
        return -1;
    }

 	listen(jack_fd, LISTEN_BACKLOG);
    return 0;

}

void serverRun(){
    struct sockaddr_in c_addr;
    socklen_t c_len = sizeof (c_addr);

    while(1){
        jack_fd = accept(jack_fd , (void *) &c_addr, &c_len);

    }
}
int main(int argc, char ** argv){   
    write(1, MSG_BENVINGUDA, strlen(MSG_BENVINGUDA));
    //Comprovem que el número d'arguments sigui correcte
    if(argc != 2){
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

    readConfigFileJack(&configJack, argv[1]);
    printf("IP: %s - PORT: %d \n", configJack.ip, configJack.port);

    launch_server(configJack);
    serverRun();

    close(jack_fd);
    
    

}