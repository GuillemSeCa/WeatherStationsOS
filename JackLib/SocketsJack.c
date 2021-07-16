#include "SocketsJack.h"

//Variable global
<<<<<<< HEAD
int fdSocketServer, fdSocketClient, countClients, *clientPIDs;
=======
int fdSocketServer, fdSocketClient;
>>>>>>> 269fe6ddfa32e5505d29554c3e144384edef9a1a

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
            
            //Esperar acabar thread abans d'acceptar nou thread
            //pthread_join(threadId, NULL); //Comentat perquè Jack accepti múltiples connexions alhora (servidor dedicat)
        }
	}
	
    //En cas d'error al acceptar la nova connexió, mostrem missatge informatiu
	if(fdSocketClient < 0) {
        write(1, MSG_ERR_ACCEPT, sizeof(MSG_ERR_ACCEPT));
	}

    //Alliberar i tancar variables utilitzades per aquesta connexió
    close(fdSocketClient);
    free(auxSocket);
    auxSocket = NULL;
}

//Mètode que controlarà el comportament dels threads
void *connectionHandler(void *auxSocket) {
    int numStations, numChar, i;
    char aux[2000];
    Packet paquet;
    Station *stations;
    int sock = *(int*)auxSocket;
    aux[0] = '\0';

<<<<<<< HEAD
    //Llegim el PID d'aquest Danny
    read(sock, &clientPIDs[countClients - 1], sizeof(int));

=======
>>>>>>> 269fe6ddfa32e5505d29554c3e144384edef9a1a
    //Anem llegint les dades fins que Danny es desconnecti
    while (read(fdSocketClient, &paquet, sizeof(Packet))) {
        //Si Danny es desconnecta, sortim
        if(paquet.tipus == 'Q' && strcmp(paquet.origen, "DANNY") == 0) {
            break;
        }

        //Llegim quantes estacions rebrem i reservem la memòria dinàmica necessària
        write(1, "\n", 1);
        write(1, MSG_JACK, sizeof(MSG_JACK));
        write(1, MSG_RECEIVING, sizeof(MSG_RECEIVING));
        read(sock, &numStations, sizeof(int));
        stations = (Station *) malloc(sizeof(Station) * numStations);

        //Llegim totes les estacions rebudes
        for(i = 0; i < numStations; i++) {
            //FileName
            read(sock, &numChar, sizeof(int));
            stations[i].fileName = (char *) malloc(sizeof(char) * numChar);
            read(sock, stations[i].fileName, sizeof(char)*numChar);
            stations[i].fileName[numChar] = '\0';
            //Date
            read(sock, &numChar, sizeof(int));
            stations[i].date = (char *) malloc(sizeof(char) * numChar);
            read(sock, stations[i].date, sizeof(char)*numChar);
            stations[i].date[numChar] = '\0';
            //Hour
            read(sock, &numChar, sizeof(int));
            stations[i].hour = (char *) malloc(sizeof(char) * numChar);
            read(sock, stations[i].hour, sizeof(char)*numChar);
            stations[i].hour[numChar] = '\0';
            //Temperature
            read(sock, &stations[i].temperature, sizeof(float));
            //Humidity
            read(sock, &stations[i].humidity, sizeof(int));
            //AtmosphericPressure
            read(sock, &stations[i].atmosphericPressure, sizeof(float));
            //Precipitation
            read(sock, &stations[i].precipitation, sizeof(float));
            
            //Mostrem la informació del paquet
            paquet.dades[strlen(paquet.dades)] = '\0';
            sprintf(aux, "\n%s\n%s\n%s\n%.1f\n%d\n%.1f\n%.1f\n", paquet.dades, stations[i].date, stations[i].hour, stations[i].temperature, stations[i].humidity, stations[i].atmosphericPressure, stations[i].precipitation);
            write(1, aux, strlen(aux));
        }

        //Informem a Danny de que les dades són correctes
        strcpy(paquet.origen, "JACK"); 
        paquet.origen[4] = '\0';
        paquet.tipus = 'B';
        strcpy(paquet.dades, "DADES OK");
        paquet.dades[8] = '\0';
        write(fdSocketClient, &paquet, sizeof(Packet));
    }
		
    //Alliberar i tancar variables utilitzades per aquesta connexió
	close(sock);
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
	free(auxSocket);
    auxSocket = NULL;
	
	return 0;
}

//Mètode per tancar el servidor
void closeServer() {
    free(clientPIDs);
    clientPIDs = NULL;
    close(fdSocketServer);
    close(fdSocketClient);

	exit(0);
}