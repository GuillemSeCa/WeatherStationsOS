#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "ConfigWendy.h"
#include "SignalsWendy.h"
#include "SocketsWendy.h"

//Constants
#define MSG_BENVINGUDA "\nStarting Wendy...\n\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!\n"

//Variables globals
int fdSocketServer;
int fdSocketClient;
ConfigWendy configWendy;

//the thread function
void *connection_handler(void *);

int main(int argc, char **argv) {
	//Comprovem que el número d'arguments sigui correcte
    if (argc != 2) {
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

	//Canviem el que fa per defecte el CTRL+C
    signal(SIGINT, ctrlCSignal);
	signal(SIGKILL, ctrlCSignal);

	//Missatge benvinguda
	write(1, MSG_BENVINGUDA, strlen(MSG_BENVINGUDA));

	//Llegim el fitxer de configuració per obtenir la IP i Port d'aquest server Jack
	readConfigFileWendy(&configWendy, argv[1]);
	
	//Preparem la configuració d'aquest servidor Wendy
	launchServer(configWendy);

	//Iniciem el servidor i esperem connexió dels Clients Danny
	serverRun();

	//Alliberem tota la memòria dinàmica restant i tanquem tot
    closeServer();
	
	return 0;
}