//
// Created by chawki on 30/12/18.
//

#ifndef SHELL_COMMANDES_INTERNES_H
#define SHELL_COMMANDES_INTERNES_H

#include <unistd.h>
#include <errno.h>
#include "traitement.h"


void my_cd();

void my_exit();

void my_set();

void my_unset();

void my_export();

void my_get_cmd_history(char **argv);

void my_history();

int traitement_fichier_sh(char **argv);

#endif //SHELL_COMMANDES_INTERNES_H
