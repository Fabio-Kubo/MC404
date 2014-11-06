ra145979: ra145979.o main.o
	gcc -g main.o ra145979.o -o ra145979
main.o: main.c
	gcc -g -c main.c -o main.o
ra145979.o: ra145979.s
	as -g ra145979.s -o ra145979.o
