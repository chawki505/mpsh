CC=gcc
CFLAGS = -g -Wall -std=gnu11
LDFLAGS= -lreadline

EXEC=mpsh

all: $(EXEC)

mpsh: main.o commandes_internes.o extra.o analyseur.o redirection.o
	$(CC) -o mpsh main.o commandes_internes.o extra.o analyseur.o redirection.o $(LDFLAGS)

main.o: main.c variables.h

commandes_internes.o: commandes_internes.c commandes_internes.h

extra.o: extra.c extra.h

analyseur.o: analyseur.c analyseur.h

redirection.o: redirection.c redirection.h

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)