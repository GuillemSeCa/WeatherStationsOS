#include "SocketsJack.h"

//Variable global
Station *stations;
int fdSocketServer;
int fdSocketClient;

//Mètode per configurar el servidor abans d'iniciar-lo
int launchServer(ConfigJack configJack) {
    struct sockaddr_in serverSocket;

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

//TODO: Arreglar
//Mètode per iniciar el Servidor i esperar la connexió dels Clients Danny
void serverRun() {
    int *auxSocket, estat;
    struct sockaddr_in clientSocket;
    socklen_t c_len = sizeof(clientSocket);
    Packet paquet;
    /*char dataReceivedChar[100];
    int numChar, numStations, i;
    char str[200];
    struct sockaddr_in c_addr;
    socklen_t c_len = sizeof(c_addr);*/

    //Anar acceptar noves connexions fins Ctrl+C
	write(1, MSG_JACK, sizeof(MSG_JACK));
	write(1, MSG_WAITING, sizeof(MSG_WAITING));
    //Bucle per anar realitzant accepts (crear nou file descriptor amb connexió activa)
	while((fdSocketClient = accept(fdSocketServer, (struct sockaddr *)&clientSocket, &c_len))) {	
        pthread_t threadId;
		auxSocket = malloc(1);
		*auxSocket = fdSocketClient;

        //Llegim primer paquet, amb informació sobre l'estació
        read(fdSocketClient, &paquet, sizeof(Packet));
        if(paquet.tipus == 'I' && strcmp(paquet.origen, "JACK") == 0) {
            write(1, "New Connection: ", 17);
            write(1, paquet.dades, strlen(paquet.dades));
            write(1, "\n", 1);
        }

        //Creem nou thread per la nova connexió, passant-li el socket del client
        estat = pthread_create(&threadId, NULL, connectionHandler, (void*)auxSocket);
		//En cas d'error al crear el thread, mostrem missatge informatiu
        if(estat < 0) {
			write(1, MSG_ERR_CREATE, sizeof(MSG_ERR_CREATE));
		}
		
		//Esperar acabar thread abans d'acceptar nou thread
		pthread_join(threadId, NULL);
	}
	
    //En cas d'error al acceptar la nova connexió, mostrem missatge informatiu
	if(fdSocketClient < 0) {
        write(1, MSG_ERR_ACCEPT, sizeof(MSG_ERR_ACCEPT));
	}

    //Alliberar i tancar variables utilitzades per aquesta connexió
    close(fdSocketClient);
    free(auxSocket);
    auxSocket = NULL;
    
    //Bucle infinit per anar acceptant connexions fins CTRL+C
    /*while (1) {
        write(1, MSG_JACK, sizeof(MSG_JACK));
        write(1, "Waiting...\n", sizeof(char)*12);
        counterDannys++;
        
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
    }*/
}

//Mètode que controlarà el comportament dels threads
void *connectionHandler(void *auxSocket) {
	//Get the socket descriptor
	/*int sock = *(int*)auxSocket;
	int read_size;
	char *message , client_message[2000];
	
	//Send some messages to the client
	message = "Greetings! I am your connection handler\n";
	write(sock , message , strlen(message));
	
	message = "Now type something and i shall repeat what you type \n";
	write(sock , message , strlen(message));
	
	//Receive a message from client
	while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
	{
		//Send the message back to client
		write(sock , client_message , strlen(client_message));
	}
	
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
		
    //Alliberar i tancar variables utilitzades per aquesta connexió
	close(sock);*/
	free(auxSocket);
    auxSocket = NULL;
	
	return 0;
}

//Mètode per tancar el servidor
void closeServer() {
    close(fdSocketServer);
    close(fdSocketClient);

	exit(0);
}