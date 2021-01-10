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
#define MSG_JACK "$Jack:\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"
#define MSG_ERR_BIND "Error durant el bind del port (Servidor)!\n"
#define MSG_ERR_SOCKET "Error durant la creacio del socket del Servidor!\n"

//Variables globals
int fdSocket;
int fdClientDannys[20];
int counterDannys = -1;
ConfigJack configJack;
Station *stations;

//Mètode per configurar el servidor abans d'iniciar-lo
int launchServer(ConfigJack configJack) {
    struct sockaddr_in socketAdress;

    //Obtenim el File Descriptor del Socket (sense connexió)
    fdSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Comprovem que s'hagi obtingut correctament
    if (fdSocket < 0) {
        write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
        return -1;
    }

    //Carguem l'adreça amb les dades corresponents (IP, Port, etc.)
    bzero(&socketAdress, sizeof(socketAdress));
    socketAdress.sin_family = AF_INET;
    socketAdress.sin_port = htons(configJack.port);
    socketAdress.sin_addr.s_addr = inet_addr(configJack.ip);

    //Fem el bind, per indicar al Sistema Operatiu que l'aplicació espera informació d'un port determinat
    if (bind(fdSocket, (void *) &socketAdress, sizeof(socketAdress)) < 0) {
        //En cas d'error al fer el bind, mostrem missatge informatiu i no continuem
        write(1, MSG_ERR_BIND, sizeof(MSG_ERR_BIND));
        return -1;
    }

    //Obrim el port assignat al Socket i li marquem que el màxim de connexions a acceptar seran 20
    listen(fdSocket, MAX_CONNECTIONS);
    return 0;

}

//Mètode per iniciar el Servidor i esperar la connexió dels Clients Danny
void serverRun() {
    char dataReceivedChar[100];
    int numChar, numStations, i;
    char str[200];
    struct sockaddr_in c_addr;
    socklen_t c_len = sizeof(c_addr);


    


    
    //Bucle infinit per anar acceptant connexions fins CTRL+C
    while (1) {
        write(1, MSG_JACK, sizeof(MSG_JACK));
        write(1, "Waiting...\n", sizeof(char)*12);
        counterDannys++;
        


/********************************************************************************************************************************************************************
 * 
 * FALLLLLLLLLLLLLLLLLLLLLLLLAAAAAAAAAAAAAAA AQUIIIIIIIIIIIIIIIIII
 * 
 ********************************************************************************************************************************************************************/



        //Creem un nou File Descriptor amb connexió activa 
        fdClientDannys[counterDannys] = accept(fdSocket, (void *) &c_addr, &c_len);

        
        
        
        




        //Llegim el nom de l'estació
        read(fdClientDannys[counterDannys], &numChar, sizeof(int));
        read(fdClientDannys[counterDannys], &dataReceivedChar, sizeof(char)*numChar);
        dataReceivedChar[numChar] = '\0';
        sprintf(str, "New Connection: %s\n", dataReceivedChar);
        write(1, str, strlen(str));

        //Llegim quantes estacions rebrem i reservem la memòria dinàmica necessària
        read(fdClientDannys[counterDannys], &numStations, sizeof(int));
        stations = (Station *) malloc(sizeof(Station) * numStations);
        
        //Llegim totes les estacions rebudes
        for(i = 0; i < numStations; i++) {
            //fileName
            read(fdClientDannys[counterDannys], &numChar, sizeof(int));
            stations[i].fileName = (char *) malloc(sizeof(char) * numChar);
            read(fdClientDannys[counterDannys], stations[i].fileName, sizeof(char)*numChar);
            stations[i].fileName[numChar] = '\0';
            //date
            read(fdClientDannys[counterDannys], &numChar, sizeof(int));
            stations[i].date = (char *) malloc(sizeof(char) * numChar);
            read(fdClientDannys[counterDannys], stations[i].date, sizeof(char)*numChar);
            stations[i].date[numChar] = '\0';
            //hour
            read(fdClientDannys[counterDannys], &numChar, sizeof(int));
            stations[i].hour = (char *) malloc(sizeof(char) * numChar);
            read(fdClientDannys[counterDannys], stations[i].hour, sizeof(char)*numChar);
            stations[i].hour[numChar] = '\0';
            //temperature
            read(fdClientDannys[counterDannys], &stations[i].temperature, sizeof(float));
            //humidity
            read(fdClientDannys[counterDannys], &stations[i].humidity, sizeof(int));
            //atmosphericPressure
            read(fdClientDannys[counterDannys], &stations[i].atmosphericPressure, sizeof(float));
            //precipitation
            read(fdClientDannys[counterDannys], &stations[i].precipitation, sizeof(float));
            
            printf("Estacio rebuda: %s\n %s\n %s\n %f %d %f %f\n", stations[i].fileName, stations[i].date, stations[i].hour, stations[i].temperature, stations[i].humidity, stations[i].atmosphericPressure, stations[i].precipitation);
        }
        //Alliberem memòria dinàmica
        free(stations);
        stations = NULL;
        
    }
    
    

    //Tanquem els File Descriptors dels Clients per tancar la connexió activa
    for(i = 0; i < counterDannys; i++) {
        close(fdClientDannys[i]);
    }
}

void lloydProcess(){
    key_t key;
    int shmid;
    Stations *data;
    int mode;

    if (argc > 2) {
        fprintf(stderr, "usage: shmdemo [data_to_write]\n");
        exit(1);
    }

    /* make the key: */
    key = ftok("hello.txt", 'R')

    /*  create the segment: */
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    /* attach to the segment to get a pointer to it: */
    data = shmat(shmid, NULL, 0);
    if (data == (Stations *)(-1)) {
        perror("shmat");
        exit(1);
    }

    /* read or modify the segment, based on the command line: */
    if (argc == 2) {
        printf("writing to segment: \"%s\"\n", argv[1]);
        strncpy(data, argv[1], SHM_SIZE);
    } else
        printf("segment contains: \"%s\"\n", data);

    /* detach from the segment: */
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }


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


    pid = fork();
    if (pid == 0) {
        //Child
        lloydProcess();
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

        //Tanquem el File Descriptor
        close(fdSocket);
    }


    

    return 0;
}