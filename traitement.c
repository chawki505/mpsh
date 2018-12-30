//
// Created by chawki on 30/12/18.
//

#include "traitement.h"


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
            wait(&process2);
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



void liberation_arguments(char *arguments[32]) {
    int increment = 0;
    while (arguments[increment] != NULL) {
        free(arguments[increment]);
        increment++;
    }
}




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



void traitement_ligne(char **argv) {
    traitement_espaces_debut(buffer);
    traitement_espaces_fin(buffer);

    if (strncmp(buffer, "cd", 2) == 0) {
        my_cd();
    } else if (strcmp(buffer, "exit") == 0) {
        my_exit();
    } else if (traitement_fichier_sh(argv) == 0) {}
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
        //traitement des commandes externe
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


char *scan_redirection_entrante(char *arguments[32]) {
    char *redirection = NULL;
    int increment = 0;

    while (arguments[increment] != NULL) {
        if (strcmp(arguments[increment], "<") == 0) {
            redirection = arguments[increment + 1];
            free(arguments[increment]);
            while (arguments[increment + 2] != NULL) {
                arguments[increment] = arguments[increment + 2];
                ++increment;
            }
            arguments[increment] = NULL;
        }
        ++increment;
    }
    return redirection;
}

char *scan_redirection_sortante(char *arguments[32]) {
    char *redirection = NULL;
    int increment = 0;

    while (arguments[increment] != NULL) {
        if (strcmp(arguments[increment], ">") == 0) {
            redirection = malloc(strlen(arguments[increment + 1]) + 1);
            redirection[0] = 'w';
            redirection[1] = '\0';
            strcat(redirection, arguments[increment + 1]);
            free(arguments[increment]);
            free(arguments[increment + 1]);
            while (arguments[increment + 2] != NULL) {
                arguments[increment] = arguments[increment + 2];
                ++increment;
            }
            arguments[increment] = NULL;
        } else if (strcmp(arguments[increment], ">>") == 0) {
            redirection = malloc(strlen(arguments[increment + 1]) + 1);
            redirection[0] = 'a';
            redirection[1] = '\0';
            strcat(redirection, arguments[increment + 1]);
            free(arguments[increment]);
            free(arguments[increment + 1]);
            while (arguments[increment + 2] != NULL) {
                arguments[increment] = arguments[increment + 2];
                ++increment;
            }
            arguments[increment] = NULL;
        }
        ++increment;
    }
    return redirection;
}