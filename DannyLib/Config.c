#include "Config.h"

//Mètode per llegir el fitxer de configuració
void readConfigFile(Config * config, char * path){
    int fdConfig;
    
    //Obtenim el file descriptor del fitxer de configuració
    fdConfig = open(path, O_RDONLY);
    
    //Verifiquem que s'hagi obtingut correctament
    if(fdConfig < 0){
        write(1, "Error lectura de fitxer config!\n", 33);
    //Si s'ha obtingut correctament
    } else {
        //Ens guardem la configuració del fitxer 
        config->nomEstacio = read_until(fdConfig, '\n');
        config->pathCarpeta = read_until(fdConfig, '\n');
        config->tempsRevisioFixers = atoi(read_until(fdConfig, '\n'));
        config->ipJack = read_until(fdConfig, '\n');
        config->portJack = atoi(read_until(fdConfig, '\n'));
        config->ipWendy = read_until(fdConfig, '\n');
        config->portWendy = atoi(read_until(fdConfig, '\n'));
    }

    //Tanquem el file descriptor
    close(fdConfig);
}