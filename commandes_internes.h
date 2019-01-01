
#ifndef SHELL_COMMANDES_INTERNES_H
#define SHELL_COMMANDES_INTERNES_H

#include "variables.h"


extern void my_cd();

extern void my_exit();

extern void my_set();

extern void my_unset();

extern void my_export();

extern void my_get_cmd_history(char **argv);

extern void my_history();

extern int traitement_fichier_sh(char **argv);

extern void my_type();

#endif //SHELL_COMMANDES_INTERNES_H
