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

//Estructura per guardar la configuració
typedef struct{
    char * nomEstacio;
    char * pathCarpeta;
    int tempsRevisioFixers;
    char * ipJack;
    int portJack;
    char * ipWendy;
    int portWendy;
}Config;

//Estructura per guardar les dades d'una estació
typedef struct{
    char * data;
    char * hora;
    float temperatura;
    int humitat;
    float pressioAtmosferica;
    float precipitacio;
}Estacio;

//Variables globals
Config config;
DIR * directori;
Estacio * estacio = NULL;

//Mètode per llegir un fitxer de text utilitzant FD
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

//Mètode per llegir el fitxer de configuració
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

//Mètode per llegir la informació d'una estació
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

//Mètode per eliminar memòria
void freeMemoria(Estacio * estacio){
    free(estacio);
}

//Mètode per eliminar un caràcter
void removeChar(char *str, char garbage) {
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

//Mètode per llegir la carpeta i tots els fitxers del seu interior
void readDirectori(DIR * directori){
    struct dirent * entrada;
    int countFitxers = 0, fdText = 0;
    char * nomFitxers = NULL;
    char * pathCarpeta = NULL;
    char * textTxt = NULL;
    char textFitxers[255], pathText[255];
    pathText[0] = '\0';
    
    //Obrim el directori
    pathCarpeta = (char*) malloc(sizeof(char) * (strlen(config.pathCarpeta) + 1));
    strcat(pathCarpeta, ".");
    strcat(pathCarpeta, config.pathCarpeta);
    directori = opendir(pathCarpeta);

    //Guardem memòria pels fitxers
    nomFitxers = (char*) malloc(sizeof(char) * 1);

    //Bucle per recòrrer tots els fitxers de la carpeta
    while((entrada = readdir(directori)) != NULL){
        countFitxers++;
        //Ens saltem els fitxers '.' i '..' que sempre trobarem
        if(countFitxers > 2) {
            //Si és un arxiu .txt, llegim la informació del fitxer dades
            if (strstr(entrada->d_name , ".txt")) {
                //Reservem memòria
                textTxt = (char*) malloc(sizeof(char));

                //Guardem el path del fitxer a llegir
                strcat(pathText, config.pathCarpeta);
                strcat(pathText, "/");
                strcat(pathText, entrada->d_name);
                memmove(pathText, pathText+1, strlen(pathText));

                //Obrim fitxer i el llegim
                fdText = open(pathText, O_RDONLY);
                textTxt = read_until(fdText, '\n');
                printf("%s\n", textTxt);
                //remove(pathText);

                pathText[0] = '\0';
            }

            nomFitxers = (char*) realloc(nomFitxers, sizeof(char) * strlen(entrada->d_name));
            strcat(entrada->d_name, "\n");
            strcat(nomFitxers, entrada->d_name);
        }
        /*if(strcmp(".", entrada->d_name) == 0 || strcmp("..", entrada->d_name) == 0){
            if(firstTime == 1){
                write(1, "No files available\n", 20);
                firstTime=0;
                break;
            }
        }else{*/
            /*countFitxer++;
            //firstTime=0;
            
            nomFitxers = (char*) realloc(nomFitxers, sizeof(char) * strlen(entrada->d_name));
            strcat(entrada->d_name, "\n");
            strcat(nomFitxers, entrada->d_name);
            printf("%s\n", nomFitxers);

            //Si és un arxiu .txt, llegim la informació del fitxer dades
            if(entrada->d_name[strlen(entrada->d_name)-2] == 't' && entrada->d_name[strlen(entrada->d_name)-3] == 'x' && entrada->d_name[strlen(entrada->d_name)-4] == 't' && entrada->d_name[strlen(entrada->d_name)-5] == '.'){
                strcat(pathText, config.pathCarpeta);
                removeChar(pathText, '/');
                strcat(pathText, "/");
                strcat(pathText, entrada->d_name);
                printf("%s\n", pathText);
                pathText[strlen(pathText) -1]= '\0';
                readEstacio(estacio, pathText);
            }
        }*/
    }

    //Eliminem el comptador dels dos fitxers '.' i '..'
    countFitxers = countFitxers - 2;
    //Si no hi ha fitxers dins de la carpeta
    if(countFitxers == 0) {
        write(1, "No files available\n", 20);
    //Si hi ha fitxers dins de la carpeta
    } else {
        //Mostrem els fitxers
        sprintf(textFitxers, "%d files found\n", countFitxers);
        write(1, textFitxers, strlen(textFitxers));
        write(1, nomFitxers, strlen(nomFitxers));
    }
    
    //Alliberem tota la memòria
    free(pathCarpeta);
    nomFitxers = NULL;
    free(nomFitxers);
    textTxt = NULL;
    free(textTxt);
    closedir(directori);
    close(fdText);
}

//Mètode per substituir el funcionament del signal Alarma
void alarmaSignal(){
    //Llegeix els directoris
    write(1, "Testing...\n", 12);
    readDirectori(directori);

    signal(SIGALRM, alarmaSignal);
    alarm(config.tempsRevisioFixers);
}

int main(int argc, char ** argv){   
    //Comprovem que el número d'arguments sigui correcte
    if(argc != 2){
        write(1, MSG_ERROR_ARGUMENTS, strlen(MSG_ERROR_ARGUMENTS));
        return -1;
    }

    //Missatge benvinguda
    write(1, MSG_BENVINGUDA, strlen(MSG_BENVINGUDA));

    //Llegim la informació de el fitxer de configuració
    readConfigFile(&config, argv[1]);
    write(1, "$", 1);
    write(1, config.nomEstacio, strlen(config.nomEstacio));
    write(1, ":\n", 3);
    
    //directori = opendir(config.pathCarpeta);

    signal(SIGALRM, alarmaSignal);
    alarm(1);

    while(1) pause();

    //Alliberem tota la memòria dinàmica
    freeMemoria(estacio);
}
