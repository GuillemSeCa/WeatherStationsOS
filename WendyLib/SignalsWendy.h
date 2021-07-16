/*
* Mòdul amb els mètodes que treballen amb Signals
*/
#pragma once

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "SocketsWendy.h"

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal();