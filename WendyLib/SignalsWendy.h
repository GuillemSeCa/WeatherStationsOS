/*
* Mòdul amb els mètodes que treballen amb Signals
*/
#pragma once

#define _POSIX_SOURCE
#include <signal.h>
#include "SocketsWendy.h"

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal();