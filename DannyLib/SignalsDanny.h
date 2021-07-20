/*
* Mòdul amb els mètodes que treballen amb Signals
*/
#pragma once

#include "ConfigDanny.h"
#include "SocketsDanny.h"

//Mètode per substituir el funcionament del signal Alarma
void alarmaSignal();

//Mètode per substituir el funcionament del signal CTRL+C
void ctrlCSignal();