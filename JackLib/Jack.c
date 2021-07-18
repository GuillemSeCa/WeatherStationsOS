//#define _GNU_SOURCE
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
#include "semaphore_v2.h"

//Constants
#define MAX_CONNECTIONS 20
#define MSG_BENVINGUDA "\nStarting Jack...\n\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"
#define HALLORANN_WRITING_MSG "Reescrivint la informacio a 'Hallorann.txt'\n"
#define PATH_HALLORANN "Hallorann.txt"

//Variables globals
int memCompId, *num;
ConfigJack configJack;
semaphore jackSem, lloydSem;

void writeFile() {
    int fdHallorann;

    write(1, HALLORANN_WRITING_MSG, sizeof(HALLORANN_WRITING_MSG));

    //eliminem l'anterior fitxer i creem un nou en cas d'existir
    remove(PATH_HALLORANN);
    fdHallorann = open(PATH_HALLORANN, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    /** TODO: guardar estadistiques al fitxer
    for(i = 0; i < ){

    }
    **/

    close(fdHallorann);

    //tornem a reprogramar la signal
    signal(SIGALRM, writeFile); 
    alarm(120);
}

void endLloyd(){
    //TODO: Descomentar
    /*SEM_destructor(&jackSem);
    SEM_destructor(&lloydSem);*/
}

void lloydProcess(){
    //key_t key;
    //int shmid;
    //Stations *data;
    //int mode;

    //Reprogramem la signal Alarm per a que cada 2 minuts es reescrigui el fitxer de "Hallorann.txt"
    signal(SIGALRM, writeFile); 
    alarm(120);

    //TODO: Descomentar
    /**
    signal(SIGINT, endLloyd);

    //iniciem el semaphore
    SEM_constructor_with_name(&jackSem, ftok("Jack.c", 'a'));
    SEM_constructor_with_name(&lloydSem, ftok("Jack.c", 'b'));

	SEM_init(&jackSem, 1);
    SEM_init(&lloydSem, 0);

    while(1){
        SEM_wait(&lloydSem);
        printf("DEBUG: Començo a llegir mem compartida\n");
        //TODO: Lectura de mem compartida
        sleep(5);
        printf("DEBUG: ACABO a llegir mem compartida\n");

        SEM_signal(&jackSem);
    }

    **/
    
    /*
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
    **/
}

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

    //Creem regió memòria compartida
    memCompId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|IPC_EXCL|0600);
    if (memCompId > 0) {
        //Lliguem les adreces i obtenim el punter de la memòria
        num = shmat(memCompId, 0, 0);
    } else {
        write(1, "Error al crear la regió de memòria compartida!\n", 50);
    }

    //Fork per Lloyd
    pid = fork();
    if (pid == 0) {
        
        //Child
        //TODO: Descomentar
        //lloydProcess();

        //Memòria compartida
        sleep(1);
        printf("VALOR MEMORIA: %d\n", *num);
        sleep(10);
        printf("VALOR MEMORIA: %d\n", *num);
        //Deslliguem les adreces
        shmdt(num);
    }
    else if (pid > 0) {
        //Parent

        //Missatge benvinguda
        write(1, MSG_BENVINGUDA, strlen(MSG_BENVINGUDA));

        //Llegim el fitxer de configuració per obtenir la IP i Port d'aquest server Jack
        readConfigFileJack(&configJack, argv[1]);

        //Memòria compartida
        *num = 5;

        //Preparem la configuració d'aquest servidor Jack
        launchServer(configJack);
        
        //Iniciem el servidor i esperem connexió dels Clients Danny
        serverRun();

        //Deslliguem les adreces
        shmdt(num);
        //Eliminem la regió compartida
        shmctl(memCompId, IPC_RMID, NULL);
    }

    //Alliberem tota la memòria dinàmica restant i tanquem tot
    closeServer();
    //Deslliguem les adreces
    shmdt(num);
    //Eliminem la regió compartida
    shmctl(memCompId, IPC_RMID, NULL);

    return 0;
}