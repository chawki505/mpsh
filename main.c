#include "variables.h"


//methode pour les redirection entrante <
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

//methode pour les redirection sortante > et >>
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


//methode de l'ecture de l'entré du shell
char *lecture() {
    char *tmp = NULL;
    char *lu = NULL;

    //test si c'est un fichier scripte en entre (eg. ./mpsh script.sh)
    if (global_argc > 1) {
        tmp = malloc(152);
        lu = fgets(tmp, 150, fichier);
        if (lu == NULL) {
            exit(EXIT_SUCCESS);
        }
        if (tmp[strlen(tmp) - 1] == '\n') {
            tmp[strlen(tmp) - 1] = '\0';
        }

    } else {
        //lecture prompt
        char prompt[4096];
        prompt[0]='\0';


        strcat(prompt,"\033[1;31m");
        strcat(prompt, getenv("USER"));
        strcat(prompt,"@");
        strcat(prompt,"hostname");
        strcat(prompt,"\033[0m:\033[1;34m");

        if (strncmp(getenv("PWD"), getenv("HOME"), strlen(getenv("HOME"))) == 0) {
            char *temp_home = getenv("PWD");
            temp_home = temp_home + strlen(getenv("HOME"));
            strcat(prompt, "~");
            strcat(prompt, temp_home);

        } else {
            strcat(prompt, getenv("PWD"));
        }


        strcat(prompt, "\033[0m$ ");



        tmp = readline(prompt);

    }
    return tmp;
}


int main(int argc, char *argv[], char *arge[]) {

    //save le nombre d'arguments
    global_argc = argc;

    //init hystory
    using_history();
    FILE *handle = fopen(".mpsh_history", "r");
    //test si exite sinon le creer
    if (handle == NULL) handle = fopen(".mpsh_history", "w");
    fclose(handle);
    read_history(".mpsh_history");
    stifle_history(500);
    write_history(".mpsh_history");


    //ajout des variables d'environement dans notre shell
    int increment = 0;
    while (arge[increment] != NULL) {
        char *valeur = strstr(arge[increment], "=");
        char *nom = strndup(arge[increment], strlen(arge[increment]) - strlen(valeur));
        ajout_environnement(nom, valeur + 1);
        free(nom);
        ++increment;
    }


    //bind key clavier pour afichage des commandes
    rl_bind_keyseq("\e[A", touche_fleche_haute);
    rl_bind_key('\t', touche_tab);
    rl_bind_keyseq("\t\t", double_touche_tab);

    if (argc > 1) {
        fichier = fopen(argv[1], "r");
        if (fichier == NULL) {
            fprintf(stderr, "Erreur ouverture %s", argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        fichier = stdin;
    }

    while (1) {
        char *ligne_saisie = lecture();
        if (ligne_saisie != NULL) {
            strcpy(buffer, ligne_saisie);
            free(ligne_saisie);
            if (isatty(fileno(fichier))) {
                add_history(buffer);
                append_history(1, ".mpsh_history");
            }
            traitement_ligne(argv);
        } else {
            exit(0);
        }
    }
}
