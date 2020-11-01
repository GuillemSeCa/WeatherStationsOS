#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MSG_BENVINGUDA "\n\nStarting Danny...\n\n"
#define MSG_ERROR_ARGUMENTS "ERROR! Falten o sobren arguments!"

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

Config config;
DIR * directori;
Estacio * estacio = NULL;

char* read_until(int fd, char end) {
    int i = 0, size;
    char c = '\0';
    char* string = (char*)malloc(sizeof(char));

    while(1) {
        size = read(fd, &c, sizeof(char));
        
        if(c != end && size > 0 && c != '&') {
            string = (char*)realloc(string, sizeof(char) * (i + 2));
            string[i++] = c;
        }else{
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

    if(fdConfig < 0){
        write(1, "Error lectura de fitxer config!\n", 33);
    }else{

        config->nomEstacio = read_until(fdConfig, '\n');
        config->pathCarpeta = read_until(fdConfig, '\n');
        config->tempsRevisioFixers = atoi(read_until(fdConfig, '\n'));
        config->ipJack = read_until(fdConfig, '\n');
        config->portJack = atoi(read_until(fdConfig, '\n'));
        config->ipWendy = read_until(fdConfig, '\n');
        config->portWendy = atoi(read_until(fdConfig, '\n'));
    }
    close(fdConfig);
}

void readEstacio(Estacio * estacio, char * path){
    int fd_estacio;

    fd_estacio = open(path, O_RDONLY);

    if(fd_estacio < 0){
        write(1, "Error lectura de fitxer estacio!\n", 34);
    }else{

        estacio = (Estacio*) malloc(sizeof(Estacio));

        if(estacio == NULL){
            write(1, "Error reserva de memoria estructura Estacio!\n", 46);
        }else{
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
    }
    close(fd_estacio);
}

void freeMemoria(Estacio * estacio){
    free(estacio);
}

void readDirectori(DIR * directori){
    struct dirent * entrada;
    int firstTime=1;
    int countFitxer=0;
    char * nomFitxers = NULL;
    char * pathCarpeta = NULL;
    char textFitxers[255];
    

    pathCarpeta = (char*) malloc(sizeof(char) * (strlen(config.pathCarpeta) + 1));
    strcat(pathCarpeta, ".");
    strcat(pathCarpeta, config.pathCarpeta);

    directori = opendir(pathCarpeta);

    nomFitxers = (char*) malloc(sizeof(char) * 1);

    while( (entrada = readdir(directori)) != NULL ){
        if(strcmp(".", entrada->d_name) == 0 || strcmp("..", entrada->d_name) == 0){
            if(firstTime == 1){
                write(1, "No files available\n", 20);
                firstTime=0;
                break;
            }
        }else{
            countFitxer++;
            firstTime=0;
            
            nomFitxers = (char*) realloc(nomFitxers, sizeof(char) * strlen(entrada->d_name));
            strcat(entrada->d_name, "\n");
            strcat(nomFitxers, entrada->d_name);

            
            //si es un arxiu .txt
            //llegim la informació del fitxer dades
            readEstacio(estacio, entrada->d_name);
        }

    }
    if(countFitxer > 0){
        sprintf(textFitxers, "%d files found\n", countFitxer);
        write(1, textFitxers, strlen(textFitxers));
        write(1, nomFitxers, strlen(nomFitxers));
    }


    //if(strlen(entrada->d_name)-2 == 't' && strlen(entrada->d_name)-3 == 'x' && strlen(entrada->d_name)-4 == 't' && strlen(entrada->d_name)-5 == '.')
    
    
    free(pathCarpeta);
    nomFitxers = NULL;
    free(nomFitxers);
    closedir(directori);
}

void alarmaSignal(){
    //llegeix els directoris
    readDirectori(directori);

    signal(SIGALRM, alarmaSignal);
    alarm(config.tempsRevisioFixers);
}

void writeTesting(){
    write(1, "Testing...\n", 12);
}

int main(int argc, char ** argv){   

    if(argc != 2){
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
    }

    write(1, MSG_BENVINGUDA, strlen(MSG_BENVINGUDA));

    //llegim la informació de el fitxer de configuració
    readConfigFile(&config, argv[1]);
    write(1, "$", 1);
    write(1, config.nomEstacio, strlen(config.nomEstacio));
    write(1, ":\n", 3);
    writeTesting();

    

    
    //directori = opendir(config.pathCarpeta);

    signal(SIGALRM, alarmaSignal);
    alarm(1);

    while(1) pause();


    //alliberem tota la memoria dinamica
    freeMemoria(estacio);
    
}
