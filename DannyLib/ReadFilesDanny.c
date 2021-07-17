#include "ReadFilesDanny.h"

//Variable global
int fdServer, fdServerWendy;
Config config;
Station *stations;

//Mètode per llegir un fitxer de text utilitzant FD fins a cert caràcter
char *readUntil(int fd, char end) {
    int i = 0, size;
    char c = '\0';
    char *string = (char *) malloc(sizeof(char));
    while (1) {
        size = read(fd, &c, sizeof(char));

        if (c != end && size > 0 && c != '&') {
            string = (char *) realloc(string, sizeof(char) * (i + 2));
            string[i++] = c;
        } else {
            //Pel \0
            i++;
            break;
        }
    }
    string[i - 1] = '\0';
    return string;
}

//Mètode per llegir un fitxer de text, retornant quan és el final del fitxer
char *readUntilEnd(int fd, char end, int *endFile) {
    int i = 0, size;
    char c = '\0';
    char *string = (char *) malloc(sizeof(char));
    while (1) {
        size = read(fd, &c, sizeof(char));

        if (c != end && size > 0 && c != '&') {
            string = (char *) realloc(string, sizeof(char) * (i + 2));
            string[i++] = c;
        } else {
            //Pel \0
            i++;
            if (size == 0) {
                *endFile = 1;
            }
            break;
        }
    }
    string[i - 1] = '\0';
    return string;
}

//Mètode per llegir la informació d'una estació, que es troba en un fitxer .txt
void readStation(Station *station, char *path, int numStation) {
    int fdStation;

    //Obtenim el file descriptor del fitxer de configuració
    fdStation = open(path, O_RDONLY);

    //Verifiquem que s'hagi obtingut correctament
    if (fdStation < 0) {
        write(1, "Error lectura de fitxer Station!\n", 34);
        //Si s'ha obtingut correctament
    } else {
        station[numStation].date = readUntil(fdStation, '\n');
        station[numStation].date[strlen(station[numStation].date)-1] = '\0';
        station[numStation].hour = readUntil(fdStation, '\n');
        station[numStation].hour[strlen(station[numStation].hour)-1] = '\0';
        station[numStation].temperature = atof(readUntil(fdStation, '\n'));
        station[numStation].humidity = atoi(readUntil(fdStation, '\n'));
        station[numStation].atmosphericPressure = atof(readUntil(fdStation, '\n'));
        station[numStation].precipitation = atof(readUntil(fdStation, '\n'));
    }

    //Tanquem el file descriptor
    close(fdStation);
}

//Mètode per enviar la informació de les estacions al servidor Jack
void sendStationsToServer(Station *stations, int numStations) {
    int i;
    char aux[1000];
    Packet paquet;

    //Enviem paquet a Jack per les dades
    write(1, "\nSending data...\n", 17);
    strcpy(paquet.origen, "DANNY"); 
	paquet.origen[5] = '\0';
    paquet.tipus = 'D';
	strcpy(paquet.dades, config.stationName);
	write(fdServer, &paquet, sizeof(Packet));

    //Enviem el número d'estacions al servidor
    write(fdServer, &numStations, sizeof(int));

    //Iterem totes les estacions i anem enviant la informació
    for(i = 0; i < numStations; i++) {
        //Creació paquet
        strcpy(paquet.origen, "DANNY"); 
        paquet.origen[5] = '\0';
        paquet.tipus = 'D';
        sprintf(aux, "%s#%s#%.1f#%d#%.1f#%.1f", stations[i].date, stations[i].hour, stations[i].temperature, stations[i].humidity, stations[i].atmosphericPressure, stations[i].precipitation);
        aux[strlen(aux)] = '\0';
        strcpy(paquet.dades, aux);
        //Enviem
        write(fdServer, &paquet, sizeof(Packet));
    }

    //Confirmem que s'hagi enviat correctament les dades
    read(fdServer, &paquet, sizeof(Packet));
    if(paquet.tipus != 'B' || strcmp(paquet.origen, "JACK") != 0 || strcmp(paquet.dades, "DADES OK") != 0) {
        write(1, "Error al enviar les dades al Servidor Jack!\n", 45);
        if(paquet.tipus == 'K' && strcmp(paquet.origen, "JACK") == 0 && strcmp(paquet.dades, "DADES KO") == 0) {
            write(1, "Dades erronies!\n", 17);
        }
    }
}

//Mètode per llegir la carpeta i tots els fitxers del seu interior
void readDirectory() {
    int countTextFiles = 0, countImageFiles = 0, i = 0, size = 0, imatgeToSend;
    char textFilePath[255], aux[255]/*, md5sumCommand[255]*/, str[255], imageFilePath[255];
    char *pathFolder = NULL;
    Packet paquet;
    off_t currentPos;
    DIR *directory;
    struct dirent *entry;
    Image *images = NULL;
    textFilePath[0] = '\0';
    aux[0]= '\0';

    //Obrim el directori
    pathFolder = (char *) malloc(sizeof(char) * (strlen(config.pathFolder) + 1));
    pathFolder[0] = '\0';
    strcat(pathFolder, ".");
    strcat(pathFolder, config.pathFolder);
    directory = opendir(pathFolder);

    //Guardem memòria per les stations i comprovem que s'hagi fet correctament
    stations = (Station *) malloc(sizeof(Station) * 1);
    if (stations == NULL) {
        write(1, "Error reserva memoria stations!\n", 33);
        return;
    }

    //Guardem memòria pel nom de les imatges i comprovem que s'hagi fet correctament
    images = (Image *) malloc(sizeof(Image) * 1);
    if (images == NULL) {
        write(1, "Error reserva memoria nom imatges!\n", 36);
        return;
    }
    
    //Bucle per recòrrer tots els fitxers de la carpeta
    while ((entry = readdir(directory)) != NULL) {
        //Ens saltem els fitxers '.' i '..' que sempre trobarem
        if (countTextFiles >= 2) {
            //Si és un arxiu .txt, llegim la informació del fitxer dades
            if (strstr(entry->d_name, ".txt")) {
                //Augmentem memòria dinàmica de la llista d'stations 
                stations = (Station *) realloc(stations, sizeof(Station) * (countTextFiles-1));

                //Guardem el nom del fitxer
                stations[countTextFiles-2].fileName = (char *) malloc(sizeof(char) * strlen(entry->d_name));
                stations[countTextFiles-2].fileName[0] = '\0';
                strcat(stations[countTextFiles-2].fileName, entry->d_name);
               
                //Guardem el path del fitxer a llegir
                strcat(textFilePath, config.pathFolder);
                strcat(textFilePath, "/");
                strcat(textFilePath, entry->d_name);
                //Eliminem el primer caràcter '/' del path, ja que aquest es troba en el fitxer de configuració però no el volem
                memmove(textFilePath, textFilePath + 1, strlen(textFilePath));

                //Llegim la informació de l'estació
                readStation(stations, textFilePath, countTextFiles-2);

                //Eliminem el fitxer
                //TODO: Descomentar (ficat per no tenir que anar creant els fitxers)
                //remove(textFilePath);

                //Reiniciem el path al fitxer .txt
                textFilePath[0] = '\0';
            }

            //Si és una imatge
            if (strstr(entry->d_name, ".jpg")) {
                //Guardem el nom de la imatge
                countImageFiles++;
                images = (Image *) realloc(images, sizeof(Image) * (countImageFiles+1));
                images[countImageFiles-1].fileName = (char *) malloc(sizeof(char) * strlen(entry->d_name));
                images[countImageFiles-1].fileName[0] = '\0';
                strcat(images[countImageFiles-1].fileName, entry->d_name);

                countTextFiles--;
            }

        }
        countTextFiles++;
    }
    countTextFiles -= 2;

    //Si no hi ha fitxers dins de la carpeta
    if (countTextFiles == 0 && countImageFiles == 0) {
        write(1, "No files available\n", 20);
    } else {
        //Informem del nombre de fitxers trobats
        sprintf(aux, "%d files found\n", (countTextFiles+countImageFiles));
        write(1, aux, strlen(aux));
    }

    //Si hi ha imatges dins de la carpeta
    if (countImageFiles > 0) {
        //Mostrem el nom de les imatges
        for (i = 0; i < countImageFiles; i++) {
            write(1, images[i].fileName, strlen(images[i].fileName));
            write(1, "\n", 1);
        }

        //Enviem paquet a Wendy per les dades
        strcpy(paquet.origen, "DANNY"); 
        paquet.origen[5] = '\0';
        paquet.tipus = 'D';
        strcpy(paquet.dades, config.stationName);
        write(fdServerWendy, &paquet, sizeof(Packet));

        //Enviem el número d'estacions al servidor
        write(fdServerWendy, &countImageFiles, sizeof(int));

        //Enviem les imatges a Wendy
        for(i = 0; i < countImageFiles; i++) {
            //./carpeta + / + nomImatge
            strcpy(imageFilePath, pathFolder);
            strcat(imageFilePath, "/\0");
            strcat(imageFilePath, images[i].fileName);

            //Obrim la imatge i la carreguem en memòria
            imatgeToSend = open(imageFilePath, O_RDONLY);
            
            //Preparem primer paquet informatiu (Name#Size#MD5) per enviar a Wendy amb el nom de la imatge
            strcpy(paquet.origen, "DANNY"); 
            paquet.origen[5] = '\0';
            paquet.tipus = 'I';
            strcpy(paquet.dades, images[i].fileName);

            //Calcular tamany imatge i afegir-ho al paquet
            currentPos = lseek(imatgeToSend, (size_t)0, SEEK_CUR);
            size = lseek(imatgeToSend, (size_t)0, SEEK_END);
            lseek(imatgeToSend, currentPos, SEEK_SET);
            sprintf(str, "#%d#", size);
            strcat(paquet.dades, str);

            //Calcular MD5SUM, afegir-ho al paquet, i enviar-lo
            //TODO: Fer md5sum correctament
            strcat(paquet.dades, "aaabbbaaabbbaaabbbaaabbbaaabbbaa\0");
            write(fdServerWendy, &paquet, sizeof(Packet));
            
            //TODO: Calculem el MD5 de la imatge i la guardem en una variable
            /*strcpy(md5sumCommand, "md5sum \0");
            strcat(md5sumCommand, imageFilePath);
        
            printf("DEBUG: paquet.dades = %s", paquet.dades);
            //Enviem el paquet amb Name#Size#MD5
            write(fdServerWendy, &paquet, sizeof(Packet)); //Paquet mida#nom#md5sum
            //MOSTRA PAQUET.dades: "DANNY#I#1200#aaabbbaaabbbaaabbbaaabbbaaabbbaa"            
            */


           //TODO: enviar la imatge en paquets ANEX
            paquet.tipus = 'F';
            while(read(imatgeToSend, &paquet.dades, sizeof(char)*100) > 0 && size > 100) {
                write(fdServerWendy, &paquet, sizeof(Packet));
                
                size -= 100;
            }
            read(imatgeToSend, &paquet.dades, sizeof(char)*size);
            write(fdServerWendy, &paquet, sizeof(Packet));

            //TODO: Controlar si paquet és més petit que 1000 (l'últim si no és múltiple)
            //TODO: Llegir paquet per l'altre costat
            //TODO: enviar paquest des de Wendy de confirmacio*/

            close(imatgeToSend);
        }

        //Confirmem que s'hagi enviat correctament les dades
        read(fdServerWendy, &paquet, sizeof(Packet));
        if(paquet.tipus != 'S' || strcmp(paquet.origen, "WENDY") == 0 || strcmp(paquet.dades, "IMATGE OK") != 0) {
            write(1, "Error al enviar les dades al Servidor Wendy!\n", 46);
            if(paquet.tipus == 'R' && strcmp(paquet.origen, "WENDY") == 0 && strcmp(paquet.dades, "IMATGE KO") == 0) {
                write(1, "Dades erronies!\n", 17);
            }
        }
    }
   
    //Si hi ha fitxers de text dins de la carpeta
    if (countTextFiles > 0) {
        //Mostrem tots els fitxers llegits amb la informació corresponent
        for (i = 0; i < countTextFiles; i++) {
            write(1, stations[i].fileName, strlen(stations[i].fileName));
            write(1, "\n", 1);
        }

        for (i = 0; i < countTextFiles; i++) {
            sprintf(aux, "\n%s\n", stations[i].fileName);
            write(1, aux, strlen(aux));
            sprintf(aux, "%s\n", stations[i].date);
            write(1, aux, strlen(aux));
            sprintf(aux, "%s\n", stations[i].hour);
            write(1, aux, strlen(aux));
            sprintf(aux, "%.1f\n", stations[i].temperature);
            write(1, aux, strlen(aux));
            sprintf(aux, "%d\n", stations[i].humidity);
            write(1, aux, strlen(aux));
            sprintf(aux, "%.1f\n", stations[i].atmosphericPressure);
            write(1, aux, strlen(aux));
            sprintf(aux, "%.1f\n", stations[i].precipitation);
            write(1, aux, strlen(aux));
        }

        //Enviem la informació al servidor
        sendStationsToServer(stations, countTextFiles);
        //Missatge informatiu
        for(i = 0; i < countImageFiles; i++) {
            write(1, "Sending ", 9);
            write(1, images[i].fileName, strlen(images[i].fileName));
            write(1, "\n", 1);
        }
        write(1, "Data sent\n", 11);
    }
    
    //Alliberem tota la memòria dinàmica i tanquem tot
    free(pathFolder);
    pathFolder = NULL;
    for (i = 0; i < countTextFiles; i++) {
        free(stations[i].fileName);
        stations[i].fileName = NULL;
        free(stations[i].date);
        stations[i].date = NULL;
        free(stations[i].hour);
        stations[i].hour = NULL;
    }
    free(stations);
    stations = NULL;
    for (i = 0; i < countImageFiles; i++) {
        free(images[i].fileName);
        images[i].fileName = NULL;
    }
    free(images);
    images = NULL;
    rewinddir(directory);
    closedir(directory);
    entry = NULL;
}
