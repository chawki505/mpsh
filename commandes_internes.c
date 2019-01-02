
#include "commandes_internes.h"

//methode pour testé si c'est une commande interne
int is_cmd_in(char *cmd) {

    char *cmd_in[10] = {"cd",
                        "type",
                        "history",
                        "export",
                        "set",
                        "exit",
                        "unset",
                        "alias",
                        "unalias",
                        "help"
    };

    int compteur = 0;

    while (compteur < 8) {
        if (strcmp(cmd, cmd_in[compteur]) == 0) {
            return 0;
        }
        compteur++;
    }
    return 1;
}

//methode de la commande cd
void my_cd() {
    char *dossier = NULL;
    char *chemin = strstr(buffer, " ");
    if (chemin != NULL) {
        size_t longueur_chemin = strcspn(chemin + 1, " ");
        dossier = strndup(chemin + 1, longueur_chemin);
    } else {
        dossier = getenv("HOME");
    }
    int retour = chdir(dossier);
    if (retour != 0) {
        fprintf(stderr, "cd : %s", strerror(errno));
    } else {
        char *ancien_chemin = getenv("PWD");
        char buffer_cwd[TAILLE_BUFFER];
        getcwd(buffer_cwd, sizeof(buffer_cwd));
        setenv("PWD", buffer_cwd, 1);
        setenv("OLDPWD", ancien_chemin, 1);
    }
}

//methode de la commande type
void my_type() {
    char *tmp = strstr(buffer, " ");
    char *cmd = NULL;
    if (tmp != NULL) {
        size_t longueur_cmd = strcspn(tmp + 1, " ");
        cmd = strndup(tmp + 1, longueur_cmd);
        if (is_cmd_in(cmd) == 0) {
            printf("type : %s is a shell builtin\n", cmd);
        } else {
            printf("type : %s is a .......\n", cmd);
        }
    }
}


//methode de la commande history
void my_history() {
    if (strcmp(buffer, "history -c") == 0) {
        clear_history();
        write_history(dir_history);
    } else if (strcmp(buffer, "history") == 0) {
        HIST_ENTRY *entree_historique;
        int boucle;
        for (boucle = 1; boucle < history_length; ++boucle) {
            entree_historique = history_get(boucle);
            printf("%4d %s\n", boucle, entree_historique->line);
        }
    }
}


//fonction export
//Dans mon code, export va appeler setenv avec le nom de la variable en paramètre.
// setenv va comme son nom l'indique ajouter (ou modifier sa valeur si elle existe déjà) à l'environnement.
void my_export() {
    char *nom_variable = strstr(buffer, " ");
    if (nom_variable != NULL) {
        char *valeur_variable = NULL;
        Environnement *liste = var_environnement;
        while (liste != NULL) {
            if (strcmp(nom_variable + 1, liste->nom) == 0) {
                valeur_variable = liste->valeur;
                liste = NULL;
                setenv(nom_variable + 1, valeur_variable, 1);
            }
            if (liste != NULL) {
                liste = liste->next;
            }
        }
    }
}

//La commande unset supprime la variable.
// J'appelle unsetenv pour supprimer celle-ci de l'environnement.
// Au cas où la variable n'a pas été exportée avec export (et n'est donc pas dans l'environnement,
// mais uniquement dans la liste des variables shell), unsetvenv n'aura aucun effet,
// je n'ai donc pas besoin de m'assurer que la variable à supprimer est dans l'environnement.
void my_unset() {
    char *nom_variable = strstr(buffer, " ");
    if (nom_variable != NULL) {
        unsetenv(nom_variable + 1);
        Environnement *liste = var_environnement;
        Environnement *precedent = NULL;
        while (liste != NULL) {
            if (strcmp(nom_variable + 1, liste->nom) == 0) {
                free(liste->valeur);
                free(liste->nom);
                precedent->next = liste->next;
                free(liste);
                liste = NULL;
            }
            precedent = liste;
            if (liste != NULL) liste = liste->next;
        }
    }
}


//La commande alias_list ajoute ou modifie un alias_list
void my_alias() {
    char *var = strdup(buffer);
    var = strstr(var, " ");
    if (var != NULL) {
        char *valeur_variable = strstr(var, "=");
        if (valeur_variable != NULL) {
            char *nom_var = strndup(var + 1, strlen(var + 1) - strlen(valeur_variable));
            ajouter_alias(nom_var, valeur_variable + 1);
            free(nom_var);
        }
    } else {
        Alias *liste = alias_list;
        while (liste != NULL) {
            printf("Alias : %s=%s\n", liste->nom, liste->valeur);
            liste = liste->next;
        }
    }
}

//La commande unalias supprime un alias_list
void my_unalias() {
    char *nom_variable = strstr(buffer, " ");
    if (nom_variable != NULL) {
        Alias *courant = alias_list;
        Alias *precedent = NULL;
        while (courant != NULL) {
            if (strcmp(nom_variable + 1, courant->nom) == 0) {
                free(courant->valeur);
                free(courant->nom);
                if (precedent != NULL) {
                    precedent->next = courant->next;
                    free(courant);
                }
                    //cas supression debut
                else {
                    precedent = courant->next;
                    free(courant);
                    alias_list = precedent;
                }
            } else {
                precedent = courant;
                courant = courant->next;
            }
        }
    }
}


//methode de la commande set liste les variables du shell. Son traitement s'effectue dans la fonction traitement_ligne.
void my_set() {
    Environnement *liste = var_environnement;
    while (liste != NULL) {
        printf("%s=%s\n", liste->nom, liste->valeur);
        liste = liste->next;
    }
}

//methode de la commande exit
void my_exit() { exit(EXIT_SUCCESS); }


//methode help pour les commande internene
void my_help() {

    char *nom_commande = strstr(buffer, " ");
    if (nom_commande != NULL) {

        if (strncmp(nom_commande + 1, "cd", 2) == 0) {
            printf("cd: cd [dir]\n"
                   "    Change the shell working directory.\n"
                   "    \n"
                   "    Change the current directory to DIR.  The default DIR is the value of the\n"
                   "    HOME shell variable.\n"
                   "    Exit Status:\n"
                   "    Returns 0 if the directory is changed.\n");
        } else if (strncmp(nom_commande + 1, "alias", 5) == 0) {
            printf("alias: alias [name=value] ... ]\n"
                   "    Define or display aliases.\n"
                   "    \n"
                   "    Without arguments, `alias' prints the list of aliases in the reusable\n"
                   "    form `alias NAME=VALUE' on standard output.\n"
                   "    \n"
                   "    Otherwise, an alias is defined for each NAME whose VALUE is given.\n"
                   "    A trailing space in VALUE causes the next word to be checked for\n"
                   "    alias substitution when the alias is expanded.\n"
                   "    \n"
                   "    Exit Status:\n"
                   "    alias returns true unless a NAME is supplied for which no alias has been\n"
                   "    defined.\n");
        } else if (strncmp(nom_commande + 1, "unalias", 7) == 0) {
            printf("unalias: unalias name\n"
                   "    Remove each NAME from the list of defined aliases.\n"
                   "    \n"
                   "    Return success unless a NAME is not an existing alias.\n");
        } else if (strncmp(nom_commande + 1, "export", 6) == 0) {
            printf("export: export [name=value]\n"
                   "    Set export attribute for shell variables.\n"
                   "    \n"
                   "    Marks each NAME for automatic export to the environment of subsequently\n"
                   "    executed commands.  If VALUE is supplied, assign VALUE before exporting.\n"
                   "    \n"
                   "    Exit Status:\n"
                   "    Returns success unless an invalid option is given or NAME is invalid.\n");
        } else if (strncmp(nom_commande + 1, "set", 3) == 0) {

            printf("set: set \n"
                   "    print all environment variable of shell.\n"
                   "    \n"
                   "    Exit Status:\n"
                   "    Returns success unless an invalid option is given.\n");
        } else if (strncmp(nom_commande + 1, "exit", 4) == 0) {
            printf("exit: exit \n"
                   "    Exit Shell:\n"
                   "    Returns success.\n");
        } else if (strncmp(nom_commande + 1, "unset", 5) == 0) {
            printf("unset: unset name \n"
                   "    Delete environment variable [name] of shell.\n"
                   "    \n"
                   "    Exit Status:\n"
                   "    Returns success.\n");
        } else if (strncmp(nom_commande + 1, "help", 4) == 0) {
            printf("help: help name \n"
                   "    print help commande name of shell.\n"
                   "    \n"
                   "    Exit Status:\n"
                   "    Returns success unless an invalid option is given.\n");
        } else if (strncmp(nom_commande + 1, "history", 7) == 0) {
            printf("history: history [-c] \n"
                   "    print all historique commande of shell.\n"
                   "    \n"
                   "    Options:\n"
                   "      -c\tclear the history list by deleting all of the entries"
                   "    Exit Status:\n"
                   "    Returns success unless an invalid option is given.\n");
        } else if (strncmp(nom_commande + 1, "type", 4) == 0) {
            printf("type: type name \n"
                   "    print type of commande [name].\n"
                   "    \n"
                   "    Exit Status:\n"
                   "    Returns success unless an invalid option is given.\n");
        } else {
            printf("Use commande : man %s \n", nom_commande + 1);
        }
    }
}