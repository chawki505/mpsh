
#include "commandes_internes.h"

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


//methode pour testé si c'est une commande interne
int is_cmd_in(char *cmd) {

    char *cmd_in[8] = {"cd",
                       "type",
                       "history",
                       "export",
                       "set",
                       "exit",
                       "unset",
                       "!"
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
    } else {
        // fprintf(stderr, "type : %s", strerror(errno));
    }
}


//methode de la commande history
void my_history() {
    if (strcmp(buffer, "history -c") == 0) {
        clear_history();
        // write_history(".mpsh_history");
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

// methode de la commande !-n  n le num de la cmd dans mpsh_history
void my_get_cmd_history(char **argv) {
    if (strcmp(buffer, "!") == 0) {}

    else {
        char *endptr = NULL;

        long entier = strtol(buffer + 1, &endptr, 10);

        if (entier == 0) {
            fprintf(stderr, "%s : element non trouvé\n", buffer);
        } else {

            HIST_ENTRY *historique;
            if (entier < 0) {
                entier = history_length + entier;
            }
            if (entier < 0) {
                remove_history(history_length - 1);
                //write_history(".mpsh_history");
                write_history(dir_history);
                fprintf(stderr, "Entrée non valide\n");
            } else {
                historique = history_get((int) entier);
                if (historique != NULL) {
                    strcpy(buffer, historique->line);
                    remove_history(history_length - 1);
                    add_history(buffer);
                    //write_history(".mpsh_history");
                    write_history(dir_history);
                    traitement_ligne(argv);
                }

            }
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
// J'appelle unsetenv poursupprimer celle-ci de l'environnement.
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
            ajout_alias(nom_var, valeur_variable + 1);
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
                else{
                    precedent = courant->next;
                    free(courant);
                    alias_list=precedent;
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


//TODO: umask fonction
/*
void my_umask() {
    char num[4];
    long n1, n2, n3, flag = 0, n;

    char *tmp = strstr(buffer, " ");
    char *value = NULL;

    if (tmp != NULL) {
        size_t longueur_value = strcspn(tmp + 1, " ");
        value = strndup(tmp + 1, longueur_value);

        if ( value > 3) {
            fprintf(stderr, "%s : Erreur de syntaxe\n", buffer);
            return;
        } else {
            strcpy(num, value);
            n = atol(num);

            n1 = n / 100;
            n2 = (n - (n1 * 100)) / 10;
            n3 = (n - ((n1 * 100) + (n2 * 10)));

            if (n1 >= 0 && n1 <= 7)
                if (n2 >= 0 && n2 <= 7)
                    if (n3 >= 0 && n3 <= 7) {
                        flag = 1;
                        umask((__mode_t) n);
                    }//end if

            if (flag != 1) {
                fprintf(stderr, "%s : illegal argument for umask\n", buffer);
            }
        }
    }


}
*/


//methode de la commande exit
void my_exit() { exit(EXIT_SUCCESS); }


//traitement des instructions d'un fichier scipte sh
int traitement_fichier_sh(char **argv) {

    //ignoré le commantaire du scripte shell
    if (buffer[0] == '#') {}
        //traitement if
    else if (strncmp(buffer, "if", 2) == 0) {
        char *lecture = buffer;
        int boucle = 0;
        Environnement *var_if = malloc(sizeof(Environnement));
        var_if->nom = malloc(4);
        sprintf(var_if->nom, "%d", boucle);
        var_if->valeur = strdup(buffer);
        var_if->next = NULL;

        if (strncmp(buffer, "if [", 4) != 0) {
            fprintf(stderr, "%s : Erreur de syntaxe\n", buffer);
            exit(EXIT_FAILURE);
        }

        char *test_fin = strstr(buffer, "]");

        if (strcmp(test_fin, "]") != 0 && strcmp(test_fin, "] ") != 0) {
            fprintf(stderr, "%s : Erreur de syntaxe\n", buffer);
            exit(EXIT_FAILURE);
        }

        char *formule = strdup(buffer);
        str_replace(formule, "if [", "");
        str_replace(formule, "]", "");
        str_replace(formule, " ", "");
        str_replace(formule, "\"", "");
        size_t retour = strcspn(formule, "=");

        if (retour == strlen(formule)) {
            fprintf(stderr, "%s : Erreur de syntaxe\n", buffer);
            exit(EXIT_FAILURE);
        }

        char *arg_list_temp[TAILLE_LIST_ARGS];

        arg_list_temp[0] = strndup(formule, retour);
        arg_list_temp[1] = strdup(formule + retour + 1);
        arg_list_temp[2] = NULL;
        gestion_variables(arg_list_temp, argv);
        int test = strcmp(arg_list_temp[0], arg_list_temp[1]);
        free(arg_list_temp[1]);
        free(arg_list_temp[0]);
        free(formule);
        Environnement *liste_var_if = var_if;
        Environnement *liste = liste_var_if;

        while (lecture != NULL) {
            lecture = fgets(buffer, 150, fichier);
            traitement_espaces_debut(buffer);
            if (lecture == NULL) break;
            if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = '\0';
            if (strncmp(buffer, "fi", 2) == 0) {
                ++boucle;
                var_if = malloc(sizeof(Environnement));
                var_if->nom = malloc(4);
                sprintf(var_if->nom, "%d", boucle);
                var_if->valeur = strdup(buffer);
                var_if->next = NULL;
                liste->next = var_if;
                liste = var_if;
                lecture = NULL;
            } else {
                ++boucle;
                var_if = malloc(sizeof(Environnement));
                var_if->nom = malloc(4);
                sprintf(var_if->nom, "%d", boucle);
                var_if->valeur = strdup(buffer);
                var_if->next = NULL;
                liste->next = var_if;
                liste = var_if;
            }
        }

        if (strcmp(liste->valeur, "fi") != 0) {
            fprintf(stderr, "%s : Erreur de syntaxe\n", buffer);
            exit(EXIT_FAILURE);
        }

        liste = liste_var_if;
        liste = liste->next;
        str_replace(liste->valeur, " ", "");
        if (strcmp(liste->valeur, "then") != 0) {
            fprintf(stderr, "*%s* : Erreur de syntaxe\n", liste->valeur);
            exit(EXIT_FAILURE);
        }

        liste = liste_var_if;
        liste = liste->next;
        liste = liste->next;
        int detection_else = 0;

        while (liste != NULL) {
            if (strcmp(liste->valeur, "else") == 0) {
                detection_else = 1;
            } else if (strcmp(liste->valeur, "fi") == 0) {
            } else if (liste->valeur[0] == '#') {
            } else {
                if (test == 0) {
                    if (detection_else == 0) {
                        traitement_cmd(liste->valeur, argv);
                    }
                } else {
                    if (detection_else == 1) {
                        traitement_cmd(liste->valeur, argv);
                    }
                }
            }
            liste = liste->next;
        }
        return 0;

        //traitement boucle
    } else if (strncmp(buffer, "for", 3) == 0) {
        int boucle = 0;
        Environnement *var_if = malloc(sizeof(Environnement));
        var_if->nom = malloc(4);
        sprintf(var_if->nom, "%d", boucle);
        var_if->valeur = strdup(buffer);
        var_if->next = NULL;
        ++boucle;
        Environnement *liste_var_if = var_if;
        Environnement *liste = liste_var_if;
        char *lecture;
        while (1) {
            lecture = fgets(buffer, 150, fichier);
            if (lecture == NULL) break;
            if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = '\0';
            traitement_espaces_debut(buffer);
            traitement_espaces_fin(buffer);
            var_if = malloc(sizeof(Environnement));
            var_if->nom = malloc(4);
            sprintf(var_if->nom, "%d", boucle);
            var_if->valeur = strdup(buffer);
            var_if->next = NULL;
            liste->next = var_if;
            liste = var_if;
            ++boucle;
            if (strcmp(var_if->valeur, "done") == 0) break;
        }
        if (strcmp(liste->valeur, "done") != 0) {
            fprintf(stderr, "Erreur de syntaxe : fin de fichier prématurée\n");
            exit(EXIT_FAILURE);
        }
        liste = liste_var_if;
        liste = liste->next;
        if (strcmp(liste->valeur, "do") != 0) {
            fprintf(stderr, "Erreur de syntaxe : \"do\" non trouvé dans structure for\n");
            exit(EXIT_FAILURE);
        }
        liste = liste_var_if;
        char *args_for[TAILLE_LIST_ARGS];
        creation_liste_arguments(args_for, liste->valeur);
        boucle = 3;
        ajout_environnement(args_for[1], "0");
        ajout_environnement(args_for[1], "0");
        while (args_for[boucle] != NULL) {
            ajout_environnement(args_for[1], args_for[boucle]);
            liste = liste_var_if;
            liste = liste->next;
            liste = liste->next;
            while (liste != NULL) {
                if (strcmp(liste->valeur, "done") == 0) break;
                traitement_cmd(liste->valeur, argv);
                liste = liste->next;
            }
            ++boucle;
        }
        return 0;
    }
    return 1;
}

