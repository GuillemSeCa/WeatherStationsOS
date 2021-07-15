all: Danny Jack Wendy Borrar

ReadFilesDanny.o: DannyLib/ReadFilesDanny.c DannyLib/ReadFilesDanny.h
	gcc -c DannyLib/ReadFilesDanny.c -Wall -Wextra
ConfigDanny.o: DannyLib/ConfigDanny.c DannyLib/ConfigDanny.h
	gcc -c DannyLib/ConfigDanny.c -Wall -Wextra
SignalsDanny.o: DannyLib/SignalsDanny.c DannyLib/SignalsDanny.h
	gcc -c DannyLib/SignalsDanny.c -Wall -Wextra
SocketsDanny.o: DannyLib/SocketsDanny.c DannyLib/SocketsDanny.h
	gcc -c DannyLib/SocketsDanny.c -Wall -Wextra
Danny.o: DannyLib/Danny.c DannyLib/ReadFilesDanny.h DannyLib/ConfigDanny.h DannyLib/SignalsDanny.h DannyLib/SocketsDanny.h
	gcc -c DannyLib/Danny.c -Wall -Wextra
Danny: DannyLib/ReadFilesDanny.o DannyLib/ConfigDanny.o DannyLib/SignalsDanny.o DannyLib/SocketsDanny.o DannyLib/Danny.o
	gcc DannyLib/ReadFilesDanny.o DannyLib/ConfigDanny.o DannyLib/SignalsDanny.o DannyLib/SocketsDanny.o DannyLib/Danny.o -o Danny

ConfigJack.o: JackLib/ConfigJack.c JackLib/ConfigJack.h
	gcc -c JackLib/ConfigJack.c -Wall -Wextra
SignalsJack.o: JackLib/SignalsJack.c JackLib/SignalsJack.h
	gcc -c JackLib/SignalsJack.c -Wall -Wextra
SocketsJack.o: JackLib/SocketsJack.c JackLib/SocketsJack.h
	gcc -c JackLib/SocketsJack.c -Wall -Wextra
Jack.o: JackLib/Jack.c JackLib/ConfigJack.h JackLib/SignalsJack.h JackLib/SocketsJack.h
	gcc -c JackLib/Jack.c -Wall -Wextra
Jack: JackLib/ConfigJack.o JackLib/SignalsJack.o JackLib/SocketsJack.o JackLib/Jack.o
	gcc JackLib/Jack.o JackLib/ConfigJack.o JackLib/SignalsJack.o JackLib/SocketsJack.o -o Jack -lpthread

ConfigWendy.o: WendyLib/ConfigWendy.c WendyLib/ConfigWendy.h
	gcc -c WendyLib/ConfigWendy.c -Wall -Wextra
SignalsWendy.o: WendyLib/SignalsWendy.c WendyLib/SignalsWendy.h
	gcc -c WendyLib/SignalsWendy.c -Wall -Wextra
SocketsWendy.o: WendyLib/SocketsWendy.c WendyLib/SocketsWendy.h
	gcc -c WendyLib/SocketsWendy.c -Wall -Wextra
Wendy.o: WendyLib/Wendy.c WendyLib/ConfigWendy.h WendyLib/SignalsWendy.h WendyLib/SocketsWendy.h
	gcc -c WendyLib/Wendy.c -Wall -Wextra
Wendy: WendyLib/ConfigWendy.o WendyLib/SignalsWendy.o WendyLib/SocketsWendy.o WendyLib/Wendy.o
	gcc WendyLib/Wendy.o WendyLib/ConfigWendy.o WendyLib/SignalsWendy.o WendyLib/SocketsWendy.o -o Wendy -lpthread

Borrar.o: Borr/Borrar.c
	gcc -c Borr/Borrar.c -Wall -Wextra
Borrar: Borr/Borrar.o
	gcc Borr/Borrar.o -o Borrar