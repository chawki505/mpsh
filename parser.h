
#ifndef SHELL_PARSER_H
#define SHELL_PARSER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void str_replace(char *chaine, char *recherche, char *remplace);

void traitement_espaces_debut(char *chaine_a_traiter);

void traitement_espaces_fin(char *chaine_a_traiter);


#endif //SHELL_PARSER_H
