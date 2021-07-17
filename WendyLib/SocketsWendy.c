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
            write(1, "\nNew Connection: ", 18);
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
    int imatgefd, numImages, i, j, x, tipusDadaActual, error = 0, pos = 0, k = 0, delete = 0, size = 0;
    char aux[20], pathImage[255];
    int sock = *(int*)auxSocket;
    Packet paquet;
    Image image;
    aux[0] = '\0';

    //Llegim el PID d'aquest Danny
    read(sock, &clientPIDs[countClients - 1], sizeof(int));

    //Anem llegint les dades fins que Danny es desconnecti
    while (read(sock, &paquet, sizeof(Packet))) {
        //Si Danny es desconnecta, sortim
        if (paquet.tipus == 'Q' && strcmp(paquet.origen, "DANNY") == 0) {
            write(1, "\nDanny disconnected...\n", 24);
            break;
        }

        //Llegim quantes imatges rebrem i reservem la memòria dinàmica necessària
        write(1, "\n", 1);
        write(1, MSG_WENDY, sizeof(MSG_WENDY));
        write(1, MSG_RECEIVING, sizeof(MSG_RECEIVING));
        write(1, paquet.dades, strlen(paquet.dades));
        write(1, "\n", 1);
        read(sock, &numImages, sizeof(int));

        //Anem llegint paquets, tractem i preparem les dades, i les guardem
        for (i = 0; i < numImages; i++) {
            //Lectura de el paquet inicial "Name#Size#MD5"
            read(sock, &paquet, sizeof(Packet));

            //Reservem memòria necessària
            image.fileName = (char *) malloc(sizeof(char) * 31);
            image.md5sum = (char *) malloc(sizeof(char) * 33);

            if(paquet.tipus == 'I' && strcmp(paquet.origen, "DANNY") == 0) {
                x = 0;
                tipusDadaActual = 0;
                for (j = 0; j < strlen(paquet.dades); j++) {
                    if (paquet.dades[j] == '#') {
                        tipusDadaActual++;
                        x = 0;
                    } else {
                        switch (tipusDadaActual) {
                            case 0:
                                image.fileName[x]= paquet.dades[j];
                                //Controlem format dades correcte
                                if(x > 29) {
                                    error = 1;
                                }
                                break;
                            case 1:
                                aux[x] = paquet.dades[j];
                                aux[x+1] = '\0';
                                if(x > 20) {
                                    error = 1;
                                }
                                break;
                            case 2:
                                image.md5sum[x]= paquet.dades[j];
                                if(x > 31) {
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
                    image.fileName[strlen(image.fileName)] = '\0';
                    image.md5sum[31] = '\0';
                    image.size = atoi(aux);

                    //Mostrem per pantalla la informació de l'imatge rebuda
                    write(1, image.fileName, strlen(image.fileName));
                    write(1, "\n", 1);
                }
            } else {
                error = 1;
            }

            //Reservem memòria per rebre la imatge
            image.data = (char *) malloc(sizeof(char) * image.size+1);
            image.data[0] = '\0';
            size = image.size;

            //Anem llegint i obtenint tots els paquets dividits amb els bytes de la imatge
            pos = 0;
            delete = 0;
            while (size > 100) {
                read(sock, &paquet, sizeof(Packet));
                for(k=0; k < 100; k++){
                    image.data[pos] = paquet.dades[k];
                    image.data[pos+1] = '\0';
                    pos++;
                }
                size -= 100;
                delete++;
            }
            //Llegim últim paquet amb les dades restants (menys de 100 bytes)
            read(sock, &paquet, sizeof(Packet));
            for (k = 0; k < size; k++) {
                image.data[pos] = paquet.dades[k];
            }

            //Path de Barry on guardar les imatges
            pathImage[0] = '\0';
            strcat(pathImage, "Barry\0");
            strcat(pathImage, "/\0");
            strcat(pathImage, image.fileName);

            //Guardem la imatge al directori
            imatgefd = open(pathImage, O_WRONLY | O_APPEND | O_CREAT, 0644);
            write(imatgefd, image.data, image.size);

            //Alliberem el que ja no faci falta
            free(image.fileName);
            image.fileName = NULL;
            free(image.md5sum);
            image.md5sum = NULL;
            free(image.data);
            image.data = NULL;

            close(imatgefd);
        }

        //Informem a Danny de que les dades són correctes o incorrectes
        if (error == 0) {
            strcpy(paquet.origen, "JACK"); 
            paquet.origen[4] = '\0';
            paquet.tipus = 'S';
            strcpy(paquet.dades, "IMATGE OK");
            paquet.dades[9] = '\0';
            write(sock, &paquet, sizeof(Packet));
        } else {
            strcpy(paquet.origen, "JACK"); 
            paquet.origen[4] = '\0';
            paquet.tipus = 'R';
            strcpy(paquet.dades, "IMATGE KO");
            paquet.dades[9] = '\0';
            write(sock, &paquet, sizeof(Packet));
        }
        error = 0;
    }
		
    //Alliberar i tancar variables utilitzades per aquesta connexió
	close(sock);
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