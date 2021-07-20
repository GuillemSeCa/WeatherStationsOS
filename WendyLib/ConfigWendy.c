#include "ConfigWendy.h"

//Mètode per llegir un fitxer de text utilitzant FD fins a cert caràcter
char *readUntil(int fd, char end)
{
    int i = 0, size;
    char c = '\0';
    char *string = (char *)malloc(sizeof(char));
    while (1)
    {
        size = read(fd, &c, sizeof(char));

        if (c != end && size > 0 && c != '&')
        {
            string = (char *)realloc(string, sizeof(char) * (i + 2));
            string[i++] = c;
        }
        else
        {
            //Pel \0
            i++;
            break;
        }
    }
    string[i - 1] = '\0';
    return string;
}

//Mètode per llegir el fitxer de configuració
void readConfigFileWendy(ConfigWendy *config, char *path)
{
    int fdConfig;

    //Obtenim el file descriptor del fitxer de configuració
    fdConfig = open(path, O_RDONLY);

    //Verifiquem que s'hagi obtingut correctament
    if (fdConfig < 0)
    {
        write(1, "Error lectura de fitxer config!\n", 33);
        //Si s'ha obtingut correctament
    }
    else
    {
        //Ens guardem la configuració del fitxer
        config->ip = readUntil(fdConfig, '\n');
        config->port = atoi(readUntil(fdConfig, '\n'));
    }

    //Tanquem el file descriptor
    close(fdConfig);
}