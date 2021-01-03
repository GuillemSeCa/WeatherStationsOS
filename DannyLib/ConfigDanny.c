#include "ConfigDanny.h"

//Mètode per llegir el fitxer de configuració
void readConfigFile(Config *config, char *path) {
    int fdConfig;

    //Obtenim el file descriptor del fitxer de configuració
    fdConfig = open(path, O_RDONLY);

    //Verifiquem que s'hagi obtingut correctament
    if (fdConfig < 0) {
        write(1, "Error lectura de fitxer config!\n", 33);
    //Si s'ha obtingut correctament
    } else {
        //Ens guardem la configuració del fitxer 
        config->nomEstacio = readUntil(fdConfig, '\n');
        config->pathCarpeta = readUntil(fdConfig, '\n');
        config->tempsRevisioFixers = atoi(readUntil(fdConfig, '\n'));
        config->ipJack = readUntil(fdConfig, '\n');
        config->portJack = atoi(readUntil(fdConfig, '\n'));
        config->ipWendy = readUntil(fdConfig, '\n');
        config->portWendy = atoi(readUntil(fdConfig, '\n'));
    }

    //Tanquem el file descriptor
    close(fdConfig);
}