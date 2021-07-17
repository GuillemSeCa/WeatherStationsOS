#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/shm.h>
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
#include "SignalsJack.h"
#include "SocketsJack.h"

//Constants
#define MAX_CONNECTIONS 20
#define MSG_BENVINGUDA "\nStarting Jack...\n\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"

//Variables globals
ConfigJack configJack;

int main(int argc, char **argv) {
    pid_t pid;

    //Comprovem que el número d'arguments sigui correcte
    if (argc != 2) {
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

    //Canviem el que fa per defecte el CTRL+C
    signal(SIGINT, ctrlCSignal);
    signal(SIGKILL, ctrlCSignal);

    //Fork per Lloyd
    pid = fork();
    if (pid == 0) {
        //Child
        //lloydProcess();
        raise(SIGKILL);
    }
    else if (pid > 0) {
        //Parent
        
        //Missatge benvinguda
        write(1, MSG_BENVINGUDA, strlen(MSG_BENVINGUDA));

        //Llegim el fitxer de configuració per obtenir la IP i Port d'aquest server Jack
        readConfigFileJack(&configJack, argv[1]);

        //Preparem la configuració d'aquest servidor Jack
        launchServer(configJack);
        
        //Iniciem el servidor i esperem connexió dels Clients Danny
        serverRun();
    }

    //Alliberem tota la memòria dinàmica restant i tanquem tot
    closeServer();

    return 0;
}

//TODO: Fase 3
/*
void lloydProcess(){
    key_t key;
    int shmid;
    Stations *data;
    int mode;

    if (argc > 2) {
        fprintf(stderr, "usage: shmdemo [data_to_write]\n");
        exit(1);
    }

    //make the key: 
    key = ftok("hello.txt", 'R');

    // create the segment: 
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    // attach to the segment to get a pointer to it:
    data = shmat(shmid, NULL, 0);
    if (data == (Stations *)(-1)) {
        perror("shmat");
        exit(1);
    }

    // read or modify the segment, based on the command line:
    if (argc == 2) {
        printf("writing to segment: \"%s\"\n", argv[1]);
        strncpy(data, argv[1], SHM_SIZE);
    } else
        printf("segment contains: \"%s\"\n", data);

    //detach from the segment: 
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }


}
*/