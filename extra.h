#ifndef SHELL_EXTRA_H
#define SHELL_EXTRA_H

#include "variables.h"

extern void ajout_environnement(char *nom_variable, char *valeur_variable);
extern void ajout_alias(char *nom_variable, char *valeur_variable);


extern void gestion_variables(char *arguments[], char **argv);

extern int touche_fleche_haute();

extern int touche_tab();

extern int double_touche_tab();

extern int touche_ctrl_c();

extern char *get_ve_value();

#endif //SHELL_EXTRA_H
