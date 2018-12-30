

#ifndef SHELL_EXTRA_H
#define SHELL_EXTRA_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glob.h>
#include <readline/readline.h>
#include <readline/history.h>


typedef struct Environnement {
    char *nom;
    char *valeur;
    struct Environnement *next;
} Environnement;

Environnement *var_environnement;

void ajout_environnement(char *nom_variable, char *valeur_variable);

void gestion_variables(char *arguments[32], char **argv, int global_argc);

int touche_fleche_haute();

int touche_tab();

int double_touche_tab();


#endif //SHELL_EXTRA_H