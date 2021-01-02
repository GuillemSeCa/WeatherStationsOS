all: Danny Jack
ReadFiles.o: DannyLib/ReadFiles.c DannyLib/ReadFiles.h
	gcc -c DannyLib/ReadFiles.c -Wall -Wextra
Config.o: DannyLib/Config.c DannyLib/Config.h
	gcc -c DannyLib/Config.c -Wall -Wextra
Signals.o: DannyLib/Signals.c DannyLib/Signals.h
	gcc -c DannyLib/Signals.c -Wall -Wextra
Danny.o: DannyLib/Danny.c DannyLib/ReadFiles.h DannyLib/Config.h DannyLib/Signals.h
	gcc -c DannyLib/Danny.c -Wall -Wextra
Danny: DannyLib/ReadFiles.o DannyLib/Config.o DannyLib/Signals.o DannyLib/Danny.o
	gcc DannyLib/ReadFiles.o DannyLib/Config.o DannyLib/Signals.o DannyLib/Danny.o -o Danny
ReadConfigJack.o: JackLib/ReadConfigJack.c JackLib/ReadConfigJack.h
	gcc -c JackLib/ReadConfigJack.c -Wall -Wextra
Jack.o: JackLib/Jack.c JackLib/ReadConfigJack.h
	gcc -c JackLib/Jack.c -Wall -Wextra
Jack: JackLib/ReadConfigJack.o JackLib/Jack.o
	gcc JackLib/Jack.o JackLib/ReadConfigJack.o -o Jack




#gcc main.c Config.c Signals.c -o Danny -Wall -Wextra
