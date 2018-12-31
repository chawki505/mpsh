#ifndef SHELL_VARIABLES_H
#define SHELL_VARIABLES_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <glob.h>
#include <readline/readline.h>
#include <readline/history.h>


#include "commandes_internes.h"
#include "extra.h"
#include "analyseur.h"

typedef struct Environnement {
    char *nom;
    char *valeur;
    struct Environnement *next;
} Environnement;

Environnement *var_environnement;

char *arg_list[32], *arg_list2[32];
char buffer[250];
int global_argc;
FILE *fichier;


extern char *scan_redirection_entrante(char *arguments[32]);

extern char *scan_redirection_sortante(char *arguments[32]);


#endif //SHELL_VARIABLES_H
