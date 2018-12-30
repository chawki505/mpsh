//
// Created by chawki on 30/12/18.
//

#ifndef SHELL_TRAITEMENT_H
#define SHELL_TRAITEMENT_H

#include <sys/wait.h>
#include "extra.h"
#include "parser.h"
#include "commandes_internes.h"


char *arg_list[32], *arg_list2[32];
char buffer[250];
int global_argc;
FILE *fichier;


void traitement_cmd(char *commande, char **argv);

void creation_liste_arguments(char *arguments[32], char *commande);

void liberation_arguments(char *arguments[32]);

void traitement_joker(char *arguments[32]);

void traitement_ligne(char **argv);

char *scan_redirection_entrante(char *arguments[32]);

char *scan_redirection_sortante(char *arguments[32]);

#endif //SHELL_TRAITEMENT_H
