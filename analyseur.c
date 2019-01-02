
#include "analyseur.h"


//methode pour remplacer une chaine avec une autre chaine
void str_replace(char *chaine, char *recherche, char *remplace) {
    int nbre = 0;
    char *p = chaine;
    char *tmp = strstr(p, recherche);
    while (tmp != NULL) {
        ++nbre;
        p = tmp + strlen(recherche);
        tmp = strstr(p, recherche);
    }
    if (nbre > 0) {
        char *chaine_copie = malloc(strlen(chaine) - (strlen(recherche) * nbre) + (strlen(remplace) * nbre) + 1);
        chaine_copie[0] = '\0';
        p = chaine;
        tmp = strstr(p, recherche);
        while (tmp != NULL) {
            strncat(chaine_copie, p, tmp - p);
            strcat(chaine_copie, remplace);
            p = tmp + strlen(recherche);
            tmp = strstr(p, recherche);
        }
        strcat(chaine_copie, p);
        strcpy(chaine, chaine_copie);
        free(chaine_copie);
    }
}


//suprime les espace au debut de la chaine
void traitement_espaces_debut(char *chaine_a_traiter) {
    char *nouvelle_chaine = chaine_a_traiter;
    while (nouvelle_chaine[0] == ' ') {
        ++nouvelle_chaine;
    }
    memmove(chaine_a_traiter, nouvelle_chaine, strlen(nouvelle_chaine) + 1);
}

//suprime les espaces en fin de chaine et ajouter ajouter le caractere de fin
void traitement_espaces_fin(char *chaine_a_traiter) {
    while (chaine_a_traiter[strlen(chaine_a_traiter) - 1] == ' ') {
        chaine_a_traiter[strlen(chaine_a_traiter) - 1] = '\0';
    }
}


//traitement d'une commande
void traitement_cmd(char *commande, char **argv) {
    char *cmd1, *cmd2 = NULL;
    char *fichier_redirection_sortante, *fichier_redirection_entrante;
    char *fichier_redirection_sortante2 = NULL, *fichier_redirection_entrante2 = NULL;
    int pipefd[2];

    //traitement des erreurs d'ecriture de commande si existe
    str_replace(commande, "\n", "");

    //str_replace(commande, " <", "<");
    //str_replace(commande, "< ", "<");
    //str_replace(commande, "<", " < ");

    //str_replace(commande, " >", ">");
    //str_replace(commande, "> ", ">");
    //str_replace(commande, ">", " > ");


    //str_replace(commande, " >  > ", " >> ");
    str_replace(commande, "| ", "|");
    str_replace(commande, " |", "|");
    str_replace(commande, "|", " | ");

    //test if existe pip
    char *existe_pipe = strstr(commande, " | ");

    //decouper la commande en 2 commande sinon ne rien faire
    if (existe_pipe != NULL) {
        //decouper la commande en 2 commande sinon ne rien faire
        cmd1 = strndup(commande, strlen(commande) - strlen(existe_pipe));
        cmd2 = strdup(existe_pipe + 3);
    } else {
        cmd1 = strdup(commande);
    }


    //creation de la liste des argument de la commande 1
    creation_liste_arguments(arg_list, cmd1);

    //enregistrer les variables si elle existe dans la ligne de commande
    gestion_variables(arg_list, argv);

    //traitement si existe etoile dans la commande (eg. ls *.c)
    traitement_joker(arg_list);

    //traitement si il existe des redirection > ou >> ou <
    fichier_redirection_sortante = scan_redirection_sortante(arg_list);
    fichier_redirection_entrante = scan_redirection_entrante(arg_list);

    //test si il existe une 2eme commande du pipe pour faire le meme traitement que la commande 1
    if (cmd2 != NULL) {
        creation_liste_arguments(arg_list2, cmd2);
        gestion_variables(arg_list2, argv);
        traitement_joker(arg_list2);
        fichier_redirection_sortante2 = scan_redirection_sortante(arg_list2);
        fichier_redirection_entrante2 = scan_redirection_entrante(arg_list2);
    }

    pipe(pipefd);

    pid_t process1 = fork();
    int status1;

    if (process1 == 0) {

        if (cmd2 != NULL) {
            dup2(pipefd[1], STDOUT_FILENO);
        }

        if (fichier_redirection_entrante != NULL) {
            FILE *handler = freopen(fichier_redirection_entrante, "r", stdin);

            if (handler == NULL) {
                fprintf(stderr, "%s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

        if (fichier_redirection_sortante != NULL) {
            char *type_redirection = strndup(fichier_redirection_sortante, 1);
            FILE *handler = NULL;

            if (redirection_err == 1) {
                handler = freopen(fichier_redirection_sortante + 1, type_redirection, stderr);
                redirection_err = 0;

            } else {
                handler = freopen(fichier_redirection_sortante + 1, type_redirection, stdout);
            }


            if (handler == NULL) {
                fprintf(stderr, "%s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            free(type_redirection);
        }

        int retour = execvp(arg_list[0], arg_list);

        if (retour == -1) {
            fprintf(stderr, "%s : %s\n", *arg_list, strerror(errno));
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }


    //wait(&process1);
    if (waitpid(process1, &status1, 0) == -1) {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status1)) {
        const int es = WEXITSTATUS(status1);
        char ess[10];
        snprintf(ess,10, "%d", es);
        ajout_environnement("?", ess);
    }


    // si existe pip
    if (cmd2 != NULL) {

        pid_t process2 = fork();
        int status2;

        if (process2 == 0) {

            dup2(pipefd[0], STDIN_FILENO);

            if (fichier_redirection_entrante2 != NULL) {

                FILE *handler = freopen(fichier_redirection_entrante2, "r", stdin);

                if (handler == NULL) {
                    fprintf(stderr, "%s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }

            if (fichier_redirection_sortante2 != NULL) {

                char *type_redirection = strndup(fichier_redirection_sortante, 1);
                FILE *handler = NULL;
                if (redirection_err == 1) {
                    handler = freopen(fichier_redirection_sortante2 + 1, type_redirection, stderr);
                    redirection_err = 0;
                } else {
                    handler = freopen(fichier_redirection_sortante2 + 1, type_redirection, stdout);
                }

                if (handler == NULL) {
                    fprintf(stderr, "%s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                free(type_redirection);
            }

            int retour = execvp(arg_list2[0], arg_list2);

            if (retour == -1) {
                fprintf(stderr, "%s", *argv);
                fprintf(stderr, "%s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }
        //wait(&process2);

        if (waitpid(process2, &status2, 0) == -1) {
            perror("waitpid failed");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status2)) {
            const int es = WEXITSTATUS(status2);
            char ess[10];
            snprintf(ess,10, "%d", es);
            ajout_environnement("?", ess);
        }

    }


    if (cmd2 != NULL) {
        if (fichier_redirection_sortante2 != NULL) free(fichier_redirection_sortante2);
        if (fichier_redirection_entrante2 != NULL) free(fichier_redirection_entrante2);
        liberation_arguments(arg_list2);
        free(cmd2);
    }
    if (fichier_redirection_entrante != NULL) free(fichier_redirection_entrante);
    if (fichier_redirection_sortante != NULL) free(fichier_redirection_sortante);
    liberation_arguments(arg_list);
    free(cmd1);
}


//methode pour creer un liste des arguments de la commande donner en entrer
void creation_liste_arguments(char *arguments[32], char *commande) {
    int boucle, increment;
    size_t longueur;

    for (boucle = 0; boucle < TAILLE_LIST_ARGS; ++boucle) {
        arguments[boucle] = NULL;
    }

    longueur = strcspn(commande, " \"");
    arguments[0] = strndup(commande, longueur);
    commande = commande + longueur;
    increment = 1;

    while (strlen(commande) > 0) {
        if (commande[0] == ' ') ++commande;
        char *separateur = " ";
        if (commande[0] == '"') separateur = "\"";
        if (strcmp(separateur, "\"") == 0) ++commande;
        longueur = strcspn(commande, separateur);
        arguments[increment] = strndup(commande, longueur);
        commande = commande + longueur;
        if (strcmp(separateur, "\"") == 0) ++commande;
        ++increment;
    }
}


//vider la memoire des arguments
void liberation_arguments(char *arguments[TAILLE_LIST_ARGS]) {
    int increment = 0;
    while (arguments[increment] != NULL) {
        free(arguments[increment]);
        increment++;
    }
}

//traitement du caractere *
void traitement_joker(char *arguments[TAILLE_LIST_ARGS]) {
    char *arg_list_tmp[TAILLE_LIST_ARGS];

    int increment = 0;
    int increment_tmp = 0;
    while (arguments[increment] != NULL) {
        size_t longueur_sans_asterisque = strcspn(arguments[increment], "*");
        char *tmp = strstr(arguments[increment], "*");
        if (tmp != NULL) {
            if (longueur_sans_asterisque != 0) tmp = tmp - longueur_sans_asterisque;
            glob_t g;
            int retour_glob = glob(tmp, 0, NULL, &g);
            if (retour_glob == 0) {
                int boucle;
                for (boucle = 0; boucle < g.gl_pathc; ++boucle) {
                    arg_list_tmp[increment_tmp] = strdup(g.gl_pathv[boucle]);
                    ++increment_tmp;
                }
                free(arguments[increment]);
            } else {
                arg_list_tmp[increment_tmp] = arguments[increment];
                ++increment_tmp;
            }
            globfree(&g);
        } else {
            arg_list_tmp[increment_tmp] = arguments[increment];
            ++increment_tmp;
        }
        ++increment;
    }
    arg_list_tmp[increment_tmp] = NULL;
    increment = 0;
    while (arg_list_tmp[increment] != NULL) {
        arguments[increment] = arg_list_tmp[increment];
        ++increment;
    }
    arguments[increment] = NULL;
}


//traitement d'une ligne de commande
void traitement_ligne(char **argv) {

    traitement_espaces_debut(buffer);
    traitement_espaces_fin(buffer);

    //traitement des commandes internes
    if (strncmp(buffer, "cd", 2) == 0) {
        my_cd();
    } else if (strcmp(buffer, "exit") == 0) {
        my_exit();
    } else if (strncmp(buffer, "type", 4) == 0) {
        my_type();
    } else if (traitement_fichier_sh(argv) == 0) {}//traitement des cmd d'un scripte sh
    else if (strcmp(buffer, "set") == 0) {
        my_set();
    } else if (strncmp(buffer, "unset", 5) == 0) {
        my_unset();
    } else if (strncmp(buffer, "export", 6) == 0) {
        my_export();
    } else if (strncmp(buffer, "!", 1) == 0) {
        my_get_cmd_history(argv);
    } else if (strcmp(buffer, "history") == 0 || strcmp(buffer, "history -c") == 0) {
        my_history();
    } else if (strncmp(buffer, "exit", 4) == 0) {
        my_exit();
        // } else if (strncmp(buffer, "$", 1) == 0) {
        //   char *nom_variable = strstr(buffer, "$");
        // char *valleur_variable = getenv(nom_variable+1);
        // printf("%s",valleur_variable);


    } else {
        //traitement des commandes externes
        char *cmd = strdup(buffer);
        char *tmp = strtok(cmd, ";");
        while (tmp != NULL) {

            char *valeur_var = strstr(tmp, "=");

            if (valeur_var != NULL) {
                char *nom_var = strndup(tmp, strlen(tmp) - strlen(valeur_var));
                ajout_environnement(nom_var, valeur_var + 1);
                free(nom_var);
            } else {
                traitement_cmd(tmp, argv);
            }
            tmp = strtok(NULL, ";");
        }
        free(cmd);
    }
}

