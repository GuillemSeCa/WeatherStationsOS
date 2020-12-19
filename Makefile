all: Danny
ReadFiles.o: ReadFiles.c ReadFiles.h
	gcc -c ReadFiles.c -Wall -Wextra
Config.o: Config.c Config.h
	gcc -c Config.c -Wall -Wextra
Signals.o: Signals.c Signals.h
	gcc -c Signals.c -Wall -Wextra
main.o: main.c ReadFiles.h Config.h Signals.h
	gcc -c main.c -Wall -Wextra
Danny: ReadFiles.o Config.o Signals.o main.o
	gcc ReadFiles.o Config.o Signals.o main.o -o Danny


#gcc main.c Config.c Signals.c -o Danny -Wall -Wextra
	