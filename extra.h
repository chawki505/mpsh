#ifndef SHELL_EXTRA_H
#define SHELL_EXTRA_H

#include "variables.h"

extern void ajout_environnement(char *nom_variable, char *valeur_variable);

extern void gestion_variables(char *arguments[32], char **argv, int global_argc);

extern int touche_fleche_haute();

extern int touche_tab();

extern int double_touche_tab();


#endif //SHELL_EXTRA_H
