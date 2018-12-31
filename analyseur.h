//
// Created by chawki on 30/12/18.
//

#ifndef SHELL_ANALYSEUR_H
#define SHELL_ANALYSEUR_H

#include "variables.h"


extern void str_replace(char *chaine, char *recherche, char *remplace);

extern void traitement_espaces_debut(char *chaine_a_traiter);

extern void traitement_espaces_fin(char *chaine_a_traiter);

extern void traitement_cmd(char *commande, char **argv);

extern void creation_liste_arguments(char *arguments[32], char *commande);

extern void liberation_arguments(char *arguments[32]);

extern void traitement_joker(char *arguments[32]);

extern void traitement_ligne(char **argv);

#endif //SHELL_ANALYSEUR_H