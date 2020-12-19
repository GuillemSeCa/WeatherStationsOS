#include "ReadFiles.h"

extern Config config;

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
            
            /*printf("%s\n", estacio[0].data);
            printf("%s\n", estacio[0].hora);
            printf("%f\n", estacio[0].temperatura);
            printf("%d\n", estacio[0].humitat);
            printf("%f\n", estacio[0].pressioAtmosferica);
            printf("%f\n", estacio[0].precipitacio);*/
        }
    }

    close(fd_estacio);
}

//Mètode per llegir la carpeta i tots els fitxers del seu interior
void readDirectori(DIR * directori){
    struct dirent * entrada;
    int countFitxers = 0, fdText = 0, fiFitxer = 0;
    char * nomFitxers = NULL;
    char * pathCarpeta = NULL;
    char * textTxt = NULL;
    char * totalTextTxt = NULL;
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
                totalTextTxt = (char*) malloc(sizeof(char));
                
                //Comencem a crear el missatge sencer del .txt amb el nom
                totalTextTxt = (char*)realloc(totalTextTxt, sizeof(char) * (strlen(entrada->d_name)+1));
                strcat(totalTextTxt, entrada->d_name);
                strcat(totalTextTxt, "\n");

                //Guardem el path del fitxer a llegir
                strcat(pathText, config.pathCarpeta);
                strcat(pathText, "/");
                strcat(pathText, entrada->d_name);
                memmove(pathText, pathText+1, strlen(pathText));

                //Obrim fitxer, el llegim i ens guardem el seu contingut
                fdText = open(pathText, O_RDONLY);
                while(fiFitxer == 0) {
                    textTxt = read_until_end(fdText, '\n', &fiFitxer);
                    totalTextTxt = (char*)realloc(totalTextTxt, sizeof(char) * 1111);
                    strcat(totalTextTxt, textTxt);
                    if(fiFitxer == 0) {
                        totalTextTxt[strlen(totalTextTxt)-1] = '\0';
                    }
                }
                remove(pathText);

                //Reiniciem el path al fitxer .txt
                pathText[0] = '\0';
            }

            nomFitxers = (char*) realloc(nomFitxers, sizeof(char) * strlen(entrada->d_name));
            strcat(entrada->d_name, "\n");
            strcat(nomFitxers, entrada->d_name);
        }
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
        write(1, "\n", 1);
        write(1, totalTextTxt, strlen(totalTextTxt));
        write(1, "\n\n", 2);
    }
    
    //Alliberem tota la memòria
    free(pathCarpeta);
    nomFitxers = NULL;
    free(nomFitxers);
    textTxt = NULL;
    free(textTxt);
    totalTextTxt = NULL;
    free(totalTextTxt);
    closedir(directori);
    close(fdText);
}

//Mètode per llegir un fitxer de text, retornant quan és el final del fitxer
char* read_until_end(int fd, char end, int * fiFitxer) {
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
            if(size == 0) {
                *fiFitxer = 1;
            }
            break;
        }
    }
    string[i - 1] = '\0';
    return string;
}