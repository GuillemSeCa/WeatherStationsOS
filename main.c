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

typedef struct{
    char * data;
    char * hora;
    float temperatura;
    int humitat;
    float pressioAtmosferica;
    float precipitacio;
}Estacio;


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

    //TODO: COMPROVACIO D'OPERTURA DE FITXER

    config->nomEstacio = read_until(fdConfig, '\n');
    config->pathCarpeta = read_until(fdConfig, '\n');
    config->tempsRevisioFixers = atoi(read_until(fdConfig, '\n'));
    config->ipJack = read_until(fdConfig, '\n');
    config->portJack = atoi(read_until(fdConfig, '\n'));
    config->ipWendy = read_until(fdConfig, '\n');
    config->portWendy = atoi(read_until(fdConfig, '\n'));
    
    close(fdConfig);
}

void readEstacio(Estacio * estacio, char * path){
    int fd_estacio;

    fd_estacio = open(path, O_RDONLY);

    //TODO: COMPROVACIO D'OPERTURA DE FITXER

    estacio = (Estacio*) malloc(sizeof(Estacio));

    //TODO: COMPROVAR RESERVA DE MEMORIA

    estacio[0].data = read_until(fd_estacio, '\n');
    estacio[0].hora = read_until(fd_estacio, '\n');
    estacio[0].temperatura = atof(read_until(fd_estacio, '\n'));
    estacio[0].humitat = atoi(read_until(fd_estacio, '\n'));
    estacio[0].pressioAtmosferica = atof(read_until(fd_estacio, '\n'));
    estacio[0].precipitacio = atof(read_until(fd_estacio, '\n'));

    printf("%s\n", estacio[0].data);
    printf("%s\n", estacio[0].hora);
    printf("%f\n", estacio[0].temperatura);
    printf("%d\n", estacio[0].humitat);
    printf("%f\n", estacio[0].pressioAtmosferica);
    printf("%f\n", estacio[0].precipitacio);


}

void freeMemoria(Estacio * estacio){
    free(estacio);
}

int main(int argc, char ** argv){
    Config config;
    Estacio * estacio;

    //llegim la informació de el fitxer de configuració
    readConfigFile(&config, argv[1]);
    
    //llegim la informació del fitxer dades
    readEstacio(estacio, "ArxiusDanny/test.txt");
    

    //alliberem tota la memoria dinamica
    freeMemoria(estacio);
    
}