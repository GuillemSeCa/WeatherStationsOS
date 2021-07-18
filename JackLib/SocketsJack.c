#include "SocketsJack.h"

//Variable global
int fdSocketServer, fdSocketClient, countClients, *clientPIDs, memCompId, *num;
semaphore jackSem, lloydSem;

//Mètode per configurar el servidor abans d'iniciar-lo
int launchServer(ConfigJack configJack) {
    struct sockaddr_in serverSocket;

    //Inicialitzem variables per guardar els clients
    countClients = 0;
    clientPIDs = (int*)malloc(sizeof(int));

    //Obtenim el File Descriptor del Socket (sense connexió)
    fdSocketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Comprovem que s'hagi obtingut correctament
    if (fdSocketServer < 0) {
		//En cas d'error al crear el socket, mostrem missatge informatiu i no continuem
        write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
        return -1;
    }
	
	//Carguem l'adreça amb les dades corresponents (IP, Port, etc.)
    bzero(&serverSocket, sizeof(serverSocket));
    serverSocket.sin_family = AF_INET;
    serverSocket.sin_addr.s_addr = inet_addr(configJack.ip);
	serverSocket.sin_port = htons(configJack.port);
	
	//Fem el bind, per indicar al Sistema Operatiu que l'aplicació espera informació d'un port determinat
    if (bind(fdSocketServer, (void *) &serverSocket, sizeof(serverSocket)) < 0) {
        //En cas d'error al fer el bind, mostrem missatge informatiu i no continuem
        write(1, MSG_ERR_BIND, sizeof(MSG_ERR_BIND));
        return -1;
    }
	
	//Obrim el port assignat al Socket i li marquem que el màxim de connexions a acceptar seran 20
    listen(fdSocketServer, MAX_CONNECTIONS);
    return 0;
}

//Mètode per iniciar el Servidor i esperar la connexió dels Clients Danny
void serverRun() {
    int *auxSocket, estat;
    struct sockaddr_in clientSocket;
    socklen_t c_len = sizeof(clientSocket);
    pthread_t threadId;
    Packet paquet;

    //Anar acceptant noves connexions fins Ctrl+C
	write(1, MSG_JACK, sizeof(MSG_JACK));
	write(1, MSG_WAITING, sizeof(MSG_WAITING));
    //Bucle per anar realitzant accepts (crear nou file descriptor amb connexió activa)
	while((fdSocketClient = accept(fdSocketServer, (struct sockaddr *)&clientSocket, &c_len))) {	
		countClients++;
        auxSocket = malloc(1);
		*auxSocket = fdSocketClient;
        clientPIDs = (int*)realloc(clientPIDs, sizeof(int) * (countClients + 1));

        //Llegim primer paquet, amb informació sobre l'estació
        read(fdSocketClient, &paquet, sizeof(Packet));
        //Si és correcte, mostrem missatge informatiu i enviem paquet de resposta
        if(paquet.tipus == 'C' && strcmp(paquet.origen, "DANNY") == 0) {
            write(1, "\nNew Connection: ", 17);
            write(1, paquet.dades, strlen(paquet.dades));
            write(1, "\n", 1);

            //Enviar paquet resposta
            strcpy(paquet.origen, "JACK"); 
            paquet.origen[4] = '\0';
            paquet.tipus = 'O';
            strcpy(paquet.dades, "CONNEXIO OK");
            paquet.dades[11] = '\0';
            write(fdSocketClient, &paquet, sizeof(Packet));

            //Creem nou thread per la nova connexió, passant-li el socket del client
            estat = pthread_create(&threadId, NULL, connectionHandler, (void*)auxSocket);
            //En cas d'error al crear el thread, mostrem missatge informatiu
            if(estat < 0) {
                write(1, MSG_ERR_CREATE, sizeof(MSG_ERR_CREATE));
            }
        }
	}
	
    //En cas d'error al acceptar la nova connexió, mostrem missatge informatiu
	if(fdSocketClient < 0) {
        write(1, MSG_ERR_ACCEPT, sizeof(MSG_ERR_ACCEPT));
	}
}

//Mètode que controlarà el comportament dels threads
void *connectionHandler(void *auxSocket) {
    int numStations, i, j, x, tipusDadaActual, error = 0;
    char aux[37], aux1[20], aux2[20], aux3[20], aux4[20];
    Packet paquet;
    Station *stations;
    int sock = *(int*)auxSocket;
    aux[0] = '\0';
    aux1[0] = '\0';
    aux2[0] = '\0';
    aux3[0] = '\0';
    aux4[0] = '\0';

    //Iniciem el semaphore
    SEM_constructor_with_name(&jackSem, ftok("Jack.c", 'a'));
    SEM_constructor_with_name(&lloydSem, ftok("Jack.c", 'b'));
    printf("RESULTAT FTOK JACK des de thread: %d\n", ftok("JackLib/Jack.c", 'a'));
    printf("RESULTAT FTOK LLOYD des de thread: %d\n", ftok("JackLib/Jack.c", 'b'));
    SEM_init(&jackSem, 0);
    SEM_init(&lloydSem, 1);

    //Llegim el PID d'aquest Danny
    read(sock, &clientPIDs[countClients - 1], sizeof(int));
    
    //Anem llegint les dades fins que Danny es desconnecti
    while (read(sock, &paquet, sizeof(Packet))) {       
        //Si Danny es desconnecta, sortim
        if (paquet.tipus == 'Q' && strcmp(paquet.origen, "DANNY") == 0) {
            write(1, "\nDanny disconnected...\n", 24);
            break;
        }

        //Llegim quantes estacions rebrem i reservem la memòria dinàmica necessària
        write(1, "\n", 1);
        write(1, MSG_JACK, sizeof(MSG_JACK));
        write(1, MSG_RECEIVING, sizeof(MSG_RECEIVING));
        read(sock, &numStations, sizeof(int));
        stations = (Station *) malloc(sizeof(Station) * numStations);

        //Mostrem nom estació
        write(1, paquet.dades, strlen(paquet.dades));
        write(1, "\n", 1);

        //Llegim totes les estacions rebudes
        for (i = 0; i < numStations; i++) {
            //Reservem memòria necessària
            stations[i].date = (char *) malloc(sizeof(char) * 11);
            stations[i].hour = (char *) malloc(sizeof(char) * 9);

            //Llegim paquet, tractem i preparem les dades, i les guardem
            read(sock, &paquet, sizeof(Packet));
            if(paquet.tipus == 'D' && strcmp(paquet.origen, "DANNY") == 0) {
                x = 0;
                tipusDadaActual = 0;
                for (j = 0; j < strlen(paquet.dades); j++) {
                    if (paquet.dades[j] == '#') {
                        tipusDadaActual++;
                        x = 0;
                    } else {
                        switch (tipusDadaActual) {
                            case 0:
                                stations[i].date[x]= paquet.dades[j];
                                //Controlem format dades correcte
                                if(x > 9) {
                                    error = 1;
                                }
                                break;
                            case 1:
                                stations[i].hour[x] = paquet.dades[j];
                                if(x > 7) {
                                    error = 1;
                                }
                                break;
                            case 2:
                                aux1[x] = paquet.dades[j];
                                if(x > 4) {
                                    error = 1;
                                }
                                break;
                            case 3:
                                aux2[x] = paquet.dades[j];
                                if(x > 2) {
                                    error = 1;
                                }
                                break;
                            case 4:
                                aux3[x] = paquet.dades[j];
                                if(x > 5) {
                                    error = 1;
                                }
                                break;
                            case 5:
                                aux4[x] = paquet.dades[j];
                                if(x > 3) {
                                    error = 1;
                                }
                                break;
                            default:
                                break;
                        }
                        x++;
                    }
                }

                if (error == 0) {
                    aux1[strlen(aux1)] = '\0';
                    aux2[strlen(aux2)] = '\0';
                    aux3[strlen(aux3)] = '\0';
                    aux4[strlen(aux4)] = '\0';
                    stations[i].date[10] = '\0';
                    stations[i].hour[8] = '\0';
                    stations[i].temperature = atof(aux1);
                    stations[i].humidity = atoi(aux2);
                    stations[i].atmosphericPressure = atof(aux3);
                    stations[i].precipitation = atof(aux4);

                    //Mostrem per pantalla la informació de l'estació rebuda
                    write(1, "\n", 1);
                    sprintf(aux, "%s\n%s\n%.1f\n%d\n%.1f\n%.1f\n", stations[i].date, stations[i].hour, stations[i].temperature, stations[i].humidity, stations[i].atmosphericPressure, stations[i].precipitation);
                    aux[strlen(aux)] = '\0';
                    write(1, aux, strlen(aux));

                    //Esperem que Lloyd no estigui accedint a la memoria dinamica
                    SEM_wait(&jackSem);
                    //Guardem a memòria compartida les dades
                    printf("DEBUG: COMENÇO a guardar les coses a memoria compartida\n");
                    *num = 44;
                    printf("DEBUG: He guardat les coses a memoria compartida\n");
                    SEM_signal(&lloydSem);
                }
            } else {
                error = 1;
            }
        }

        //Informem a Danny de que les dades són correctes o incorrectes
        if (error == 0) {
            strcpy(paquet.origen, "JACK"); 
            paquet.origen[4] = '\0';
            paquet.tipus = 'B';
            strcpy(paquet.dades, "DADES OK");
            paquet.dades[8] = '\0';
            write(sock, &paquet, sizeof(Packet));
            //TODO: Descomentar
            /**
            printf("DEBUG: COMENÇO a guardar les coses a memoria compartida\n");
            //TODO: Guardar-ho a memoria compartida
            sleep(10);
            printf("DEBUG: He guardat les coses a memoria compartida\n");
            SEM_signal(&lloydSem);
            **/
        } else {
            strcpy(paquet.origen, "JACK"); 
            paquet.origen[4] = '\0';
            paquet.tipus = 'K';
            strcpy(paquet.dades, "DADES KO");
            paquet.dades[8] = '\0';
            write(sock, &paquet, sizeof(Packet));
        }

        //Alliberem el que ja no faci falta
        error = 0;
        for(i = 0; i < numStations; i++) {
            free(stations[i].fileName);
            stations[i].fileName = NULL;
            free(stations[i].date);
            stations[i].date = NULL;
            free(stations[i].hour);
            stations[i].hour = NULL;
        }
        free(stations);
        stations = NULL;
    }
		
    //Alliberar i tancar variables utilitzades per aquesta connexió
	close(sock);
    free(auxSocket);
    auxSocket = NULL;
	
	return 0;
}

//Mètode per tancar el servidor
void closeServer() {
    shmdt(num);
    shmctl(memCompId, IPC_RMID, NULL);

    free(clientPIDs);
    clientPIDs = NULL;
    close(fdSocketServer);
    close(fdSocketClient);

    SEM_destructor(&jackSem);
    SEM_destructor(&lloydSem);

	exit(0);
}