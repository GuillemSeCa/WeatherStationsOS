#include "SocketsWendy.h"

//Variables global
int fdSocketServer, fdSocketClient, countClients, *clientPIDs;

//Mètode per configurar el servidor abans d'iniciar-lo
int launchServer(ConfigWendy configWendy) {
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
    serverSocket.sin_addr.s_addr = inet_addr(configWendy.ip);
	serverSocket.sin_port = htons(configWendy.port);
	
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

    //Anar acceptar noves connexions fins Ctrl+C
	write(1, MSG_WENDY, sizeof(MSG_WENDY));
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
            write(1, "New Connection: ", 17);
            write(1, paquet.dades, strlen(paquet.dades));
            write(1, "\n", 1);

            //Enviar paquet resposta
            strcpy(paquet.origen, "WENDY"); 
            paquet.origen[5] = '\0';
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
            //pthread_join(threadId, NULL); //Comentat perquè Wendy accepti múltiples connexions alhora (servidor dedicat)
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
    int sock = *(int*)auxSocket;

     //TODO: Lectura dels paquets del client de les imatges
    Paquet paquet;
    int size = 0;
    int newFile;
    char md5sum[33]; //un extra per el '\0'
    
    
    
    read(sock, &paquet, sizeof(Paquet));

    //TODO: definir la size
    //size = ...

    //TODO: definir el nom del fitxer

    //TODO: definir el md5sum
    if(paquet.tipus == 'C' && strcmp(paquet.origen, "DANNY") == 0) {
        newFile = open("test.png", O_WRONLY | O_CREAT);
        
        write(newFile, paquet.dades, strlen(paquet.dades));

        close(newFile);
    }


    

    //Llegim el PID d'aquest Danny
    read(sock, &clientPIDs[countClients - 1], sizeof(int));

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
    free(clientPIDs);
    clientPIDs = NULL;
    close(fdSocketServer);
    close(fdSocketClient);

	exit(0);
}