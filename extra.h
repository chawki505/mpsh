#ifndef SHELL_EXTRA_H
#define SHELL_EXTRA_H

#include "variables.h"

extern void add_environnement(char *nom_variable, char *valeur_variable);

extern void ajout_alias(char *nom_variable, char *valeur_variable);

extern void gestion_variables(char *arguments[], char **argv);

extern int touche_fleche_haute();

extern int touche_tab();

extern int double_touche_tab();

extern char *get_ve_value(char *nom_variable);

extern char *get_alias_value(char *nom_variable);


#endif //SHELL_EXTRA_H
