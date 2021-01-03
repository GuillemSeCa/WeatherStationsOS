all: Danny Jack
ReadFilesDanny.o: DannyLib/ReadFilesDanny.c DannyLib/ReadFilesDanny.h
	gcc -c DannyLib/ReadFilesDanny.c -Wall -Wextra
ConfigDanny.o: DannyLib/ConfigDanny.c DannyLib/ConfigDanny.h
	gcc -c DannyLib/ConfigDanny.c -Wall -Wextra
SignalsDanny.o: DannyLib/SignalsDanny.c DannyLib/SignalsDanny.h
	gcc -c DannyLib/SignalsDanny.c -Wall -Wextra
Danny.o: DannyLib/Danny.c DannyLib/ReadFilesDanny.h DannyLib/ConfigDanny.h DannyLib/SignalsDanny.h
	gcc -c DannyLib/Danny.c -Wall -Wextra
Danny: DannyLib/ReadFilesDanny.o DannyLib/ConfigDanny.o DannyLib/SignalsDanny.o DannyLib/Danny.o
	gcc DannyLib/ReadFilesDanny.o DannyLib/ConfigDanny.o DannyLib/SignalsDanny.o DannyLib/Danny.o -o Danny

ConfigJack.o: JackLib/ConfigJack.c JackLib/ConfigJack.h
	gcc -c JackLib/ConfigJack.c -Wall -Wextra
Jack.o: JackLib/Jack.c JackLib/ConfigJack.h
	gcc -c JackLib/Jack.c -Wall -Wextra
Jack: JackLib/ConfigJack.o JackLib/Jack.o
	gcc JackLib/Jack.o JackLib/ConfigJack.o -o Jack