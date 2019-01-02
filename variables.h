#ifndef SHELL_VARIABLES_H
#define SHELL_VARIABLES_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <glob.h>
#include <readline/readline.h>
#include <readline/history.h>


#include "commandes_internes.h"
#include "extra.h"
#include "analyseur.h"

//Type de variable d'environement
typedef struct Environnement {
    char *nom;
    char *valeur;
    struct Environnement *next;
} Environnement;

//Type alias
typedef struct Alias {
    char *nom;
    char *valeur;
    struct Alias *next;
} Alias;

//liste des VE
Alias *alias;
//liste des VE
Environnement *var_environnement;

#define TAILLE_BUFFER 1024
#define TAILLE_LIST_ARGS 32


//variable global
char *arg_list[TAILLE_LIST_ARGS], *arg_list2[TAILLE_LIST_ARGS];
char buffer[TAILLE_BUFFER];
int global_argc;
FILE *fichier;
char dir_history[TAILLE_BUFFER];
char hostname[TAILLE_BUFFER];
int redirection_err;



extern char *scan_redirection_entrante(char *arguments[]);

extern char *scan_redirection_sortante(char *arguments[]);


#endif //SHELL_VARIABLES_H
