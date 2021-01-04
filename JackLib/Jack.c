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
#include "ConfigJack.h"

//Constants
#define LISTEN_BACKLOG 20
#define MSG_BENVINGUDA "\nStarting Jack...\n\n"
#define MSG_JACK "$Jack:\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"
#define MSG_ERR_BIND "Error durant el bind del port (Servidor)!\n"
#define MSG_ERR_SOCKET "Error durant la creacio del socket del Servidor!\n"

//Variables globals
int fdDanny;
ConfigJack configJack;

int launch_server(ConfigJack configJack) {
    struct sockaddr_in s_addr;

    fdDanny = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (fdDanny < 0) {
        write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
        return -1;
    }

    bzero(&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(configJack.port);
    s_addr.sin_addr.s_addr = inet_addr(configJack.ip);
    //printf("%d\n", bind (danny_fd, (void *) &s_addr, sizeof (s_addr)));
    if (bind(fdDanny, (void *) &s_addr, sizeof(s_addr)) < 0) {
        write(1, MSG_ERR_BIND, sizeof(MSG_ERR_BIND));

        return -1;
    }

    listen(fdDanny, LISTEN_BACKLOG);
    return 0;

}

void serverRun() {
    char letra[100];
    //char str[80];
    struct sockaddr_in c_addr;
    socklen_t c_len = sizeof(c_addr);

    while (1) {
        write(1, MSG_JACK, sizeof(MSG_JACK));
        write(1, "Waiting...\n", sizeof(char)*12);
        fdDanny = accept(fdDanny, (void *) &c_addr, &c_len);

        //sprintf(str, "New Connection: %s", M_PI);
        read(fdDanny, &letra, sizeof(char)*7);
        printf("%s", letra);
        close(fdDanny);
    }
    close(fdDanny);
}

int main(int argc, char **argv) {
    write(1, MSG_BENVINGUDA, strlen(MSG_BENVINGUDA));
    //Comprovem que el número d'arguments sigui correcte
    if (argc != 2) {
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

    readConfigFileJack(&configJack, argv[1]);

    launch_server(configJack);
    serverRun();

    close(fdDanny);
}