#include "SocketsWendy.h"

//Variables global
int fdSocketServer, fdSocketClient, countClients, *clientPIDs;

//Mètode per calcular el MD5SUM d'un fitxer
void calculateMD5SUM(char md5sum[33], char imageFilePath[255]) {
    int pipeFDs[2];
    char resultExecvp[32];
    pid_t pidFork;
    char *commands[] = {"md5sum", imageFilePath, 0};
    
    //Creació Pipe per llegir les dades del fill fork
    if (pipe(pipeFDs) == -1) {
        write(1, "Error durant la creacio del pipe (Client Danny)!\n", 50);
    }
    
    //Creació fork per poder executar la comanda md5sum a través de execvp
    pidFork = fork();
    switch (pidFork) {
        //Error
        case -1:
            write(1, "Error durant la creacio del fork (Client Danny)!\n", 50);
            break;
        //Fill
        case 0:
            //Tanquem fd de lectura
            close(pipeFDs[0]);
            //Executem la comanda md5sum de la imatge i escrivim el resultat a través del pipe
            dup2(pipeFDs[1], 1);
            execvp(commands[0], commands);
            close(pipeFDs[1]);
            break;
        //Pare
        default:
            //TODO: Mirar si descomentant funciona
            //Esperar acabi fill calcular md5um
            //wait(pidFork);
            //Tanquem fd d'escriptura
            close(pipeFDs[1]);
            //Llegim el resultat a través del pipe
            read(pipeFDs[0], resultExecvp, 32);
            strcpy(md5sum, resultExecvp);
            md5sum[32] = '\0';
            close(pipeFDs[0]);
            break;
    }
}

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
    int /*imatgefd,*/ numImages, i, j, x, tipusDadaActual, error = 0, pos = 0, k = 0, delete = 0, size = 0;
    char aux[20], pathImage[255], aux2[255], md5sum[33], aux3[33];
    int sock = *(int*)auxSocket;
    Packet paquet;
    Image image;
    aux[0] = '\0';
    md5sum[0] = '\0';

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
                                image.fileName[x+1] = '\0';
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
                                image.md5sum[x] = paquet.dades[j];
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
                    image.md5sum[32] = '\0';
                    image.size = atoi(aux);

                    //Mostrem per pantalla la informació de l'imatge rebuda
                    write(1, image.fileName, strlen(image.fileName));
                    write(1, "\n", 1);
                }
            } else {
                error = 1;
            }

            //printf("DEBUG: Paquet Name#Size#MD5SUM rebut!\n");
            //Reservem memòria per rebre la imatge
            image.data = (char *) malloc(sizeof(char) * (image.size+1));
            image.data[0] = '\0';
            size = image.size;

            //printf("DEBUG: Comencem lectura imatge\n");
            //Anem llegint i obtenint tots els paquets dividits amb els bytes de la imatge
            pos = 0;
            delete = 0;
            while (size > 100) {
                read(sock, &paquet, sizeof(Packet));
                for (k=0; k < 100; k++) {
                    image.data[pos] = paquet.dades[k];
                    image.data[pos+1] = '\0';
                    pos++;
                }
                size -= 100;
                delete++;
            }
            //printf("DEBUG: Llegim ultim paquet\n");
            //Llegim últim paquet amb les dades restants (menys de 100 bytes)
            read(sock, &paquet, sizeof(Packet));
            //TODO: mirar si descomentar o no per emplenar amb \0's final com diu enunciat
            //paquet.dades[strlen(paquet.dades)] = '\0';
            for (k = 0; k < size; k++) {
                image.data[pos] = paquet.dades[k];
                pos++;
            }

            //Path de Barry on guardar les imatges
            pathImage[0] = '\0';
            strcat(pathImage, "Barry\0");
            strcat(pathImage, "/\0");
            strcat(pathImage, image.fileName);

            //Guardem la imatge al directori
            //printf("DEBUG: Me proposo a escriure! %s\n", image.fileName);
            //TODO: Descomentar
            //imatgefd = open(pathImage, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            //write(imatgefd, image.data, image.size);

            //printf("DEBUG: Comparar i calcular md5sum!\n");
            //Comprovem MD5SUM de la imatge
            strcpy(aux2, "./\0");
            strcat(aux2, pathImage);
            aux2[strlen(aux2)] = '\0';
            calculateMD5SUM(md5sum, aux2);
            strcpy(aux3, image.md5sum);
            aux3[32] = '\0';
            if (strcmp(md5sum, aux3) != 0) {
                error = 1;
            }

            //Alliberem el que ja no faci falta
            free(image.fileName);
            image.fileName = NULL;
            free(image.md5sum);
            image.md5sum = NULL;
            free(image.data);
            image.data = NULL;
            //TODO: Descomentar
            //close(imatgefd);
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