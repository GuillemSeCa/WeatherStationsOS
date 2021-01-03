/*
* Mòdul amb els mètodes que treballen amb Signals
*/
#pragma once

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
#include "ConfigDanny.h"

//Mètode per substituir el funcionament del signal Alarma
void alarmaSignal();

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal();