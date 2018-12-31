//
// Created by chawki on 30/12/18.
//

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
    char *cmd1, *cmd2;
    char *fichier_redirection_sortante, *fichier_redirection_entrante;
    char *fichier_redirection_sortante2 = NULL, *fichier_redirection_entrante2 = NULL;
    int pipefd[2];

    cmd2 = NULL;
    str_replace(commande, "\n", "");
    str_replace(commande, " <", "<");
    str_replace(commande, "< ", "<");
    str_replace(commande, "<", " < ");
    str_replace(commande, " >", ">");
    str_replace(commande, "> ", ">");
    str_replace(commande, ">", " > ");
    str_replace(commande, " >  > ", " >> ");
    str_replace(commande, "| ", "|");
    str_replace(commande, " |", "|");
    str_replace(commande, "|", " | ");
    char *tmp = strstr(commande, " | ");
    if (tmp != NULL) {
        cmd1 = strndup(commande, strlen(commande) - strlen(tmp));
        cmd2 = strdup(tmp + 3);
    } else {
        cmd1 = strdup(commande);
    }
    creation_liste_arguments(arg_list, cmd1);
    gestion_variables(arg_list, argv, global_argc);
    traitement_joker(arg_list);
    fichier_redirection_sortante = scan_redirection_sortante(arg_list);
    fichier_redirection_entrante = scan_redirection_entrante(arg_list);

    if (cmd2 != NULL) {
        creation_liste_arguments(arg_list2, cmd2);
        gestion_variables(arg_list2, argv, global_argc);
        traitement_joker(arg_list2);
        fichier_redirection_sortante2 = scan_redirection_sortante(arg_list2);
        fichier_redirection_entrante2 = scan_redirection_entrante(arg_list2);
    }

    pipe(pipefd);
    pid_t process1 = fork();
    if (process1 == 0) {
        if (cmd2 != NULL) {
            dup2(pipefd[1], STDOUT_FILENO);
        }
        if (fichier_redirection_entrante != NULL) {
            FILE *handler = freopen(fichier_redirection_entrante, "r", stdin);
            if (handler == NULL) {
                fprintf(stderr, "%s\n", strerror(errno));
                exit(0);
            }
        }
        if (fichier_redirection_sortante != NULL) {
            char *type_redirection = strndup(fichier_redirection_sortante, 1);
            FILE *handler = freopen(fichier_redirection_sortante + 1, type_redirection, stdout);
            if (handler == NULL) {
                fprintf(stderr, "%s\n", strerror(errno));
                exit(0);
            }
            free(type_redirection);
        }

        int retour = execvp(arg_list[0], arg_list);
        if (retour == -1) fprintf(stderr, "%s\n", strerror(errno));
        exit(0);

    } else {
       wait(&process1);
    }

    if (cmd2 != NULL) {
        pid_t process2 = fork();
        if (process2 == 0) {
            dup2(pipefd[0], STDIN_FILENO);
            if (fichier_redirection_entrante2 != NULL) {
                FILE *handler = freopen(fichier_redirection_entrante2, "r", stdin);
                if (handler == NULL) {
                    fprintf(stderr, "%s\n", strerror(errno));
                    exit(0);
                }
            }
            if (fichier_redirection_sortante2 != NULL) {
                char *type_redirection = strndup(fichier_redirection_sortante, 1);
                FILE *handler = freopen(fichier_redirection_sortante2 + 1, type_redirection, stdout);
                if (handler == NULL) {
                    fprintf(stderr, "%s\n", strerror(errno));
                    exit(0);
                }
                free(type_redirection);
            }
            int retour = execvp(arg_list2[0], arg_list2);
            if (retour == -1) fprintf(stderr, "%s\n", strerror(errno));
            exit(0);
        } else {
          //wait(&process2);
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

    for (boucle = 0; boucle < 32; ++boucle) {
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
void liberation_arguments(char *arguments[32]) {
    int increment = 0;
    while (arguments[increment] != NULL) {
        free(arguments[increment]);
        increment++;
    }
}

//traitement du caractere *
void traitement_joker(char *arguments[32]) {
    char *arg_list_tmp[32];

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
            } else traitement_cmd(tmp, argv);
            tmp = strtok(NULL, ";");
        }
        free(cmd);
    }
}

