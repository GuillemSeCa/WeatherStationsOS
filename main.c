#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


typedef struct{
    char * nomEstacio;
    char * pathCarpeta;
    int tempsRevisioFixers;
    char * ipJack;
    int portJack;
    char * ipWendy;
    int portWendy;
}Config;

char* read_until(int fd, char end) {
    int i = 0, size;
    char c = '\0';
    char* string = (char*)malloc(sizeof(char));

    while (1) {
        size = read(fd, &c, sizeof(char));
        
        if (c != end && size > 0 && c != '&') {
            string = (char*)realloc(string, sizeof(char) * (i + 2));
            string[i++] = c;
        } else {
            i++; //Pel \0
            break;
        }
    }

    string[i - 1] = '\0';
    return string;
}

void readConfigFile(Config * config, char * path){
    int fdConfig;
    
    fdConfig = open(path, O_RDONLY);

    config->nomEstacio = read_until(fdConfig, '\n');
    config->pathCarpeta = read_until(fdConfig, '\n');
    config->tempsRevisioFixers = atoi(read_until(fdConfig, '\n'));
    config->ipJack = read_until(fdConfig, '\n');
    config->portJack = atoi(read_until(fdConfig, '\n'));
    config->ipWendy = read_until(fdConfig, '\n');
    config->portWendy = atoi(read_until(fdConfig, '\n'));
    
    close(fdConfig);
}

void lliberarMemoria(Config * config){
    free(config);
}

int main(int argc, char ** argv){
    Config config;
    //llegim la informació de el fitxer de configuració
    readConfigFile(&config, argv[1]);
    
}