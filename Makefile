all: main.exe
	./main.exe

main.exe: main.c
	gcc -Wall -o main.exe main.c
