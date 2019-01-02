#include "variables.h"


//methode pour les redirection entrante <
char *scan_redirection_entrante(char *arguments[TAILLE_LIST_ARGS]) {
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
char *scan_redirection_sortante(char *arguments[TAILLE_LIST_ARGS]) {
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
        } else if (strcmp(arguments[increment], "2>") == 0) {

            redirection_err = 1;
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
    //Lors du lancement de l'interpréteur avec un nom de fichier en paramètre, readline attendait la saisie de quelque chose au clavier.
    //Plutôt que travailler avec stdin, je travaille sur une variable « fichier », celle-ci étant égale à stdin, en cas de mode interactif,
    // et au handle du fichier passé en paramètre (ouvert avec fopen sur le handle fichier).
    //Je lis les entrées avec une nouvelle fonction lecture(). Celle-ci utilise readline en mode interactif,
    // et utilise fgets après un malloc en mode script.
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
        char prompt[TAILLE_BUFFER];
        prompt[0] = '\0';


        strcat(prompt, "\033[1;31m");
        strcat(prompt, getenv("USER"));
        strcat(prompt, "@");
        strcat(prompt, getenv("HOSTNAME"));
        strcat(prompt, "\033[0m:\033[1;34m");

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

    char *home = getenv("HOME");
    dir_history[0] = '\0';
    strcat(dir_history, home);
    strcat(dir_history, "/.mpsh_history");

    //Si le fichier n'existe pas, l'historique ne pourra pas être lu et sauvegardé correctement.
    // Je teste donc la présence de celui-ci en l'ouvrant.
    // Si l'ouverture échoue, je crée le fichier avec fopen.
    FILE *handle = fopen(dir_history, "r");
    if (handle == NULL) handle = fopen(dir_history, "w");
    fclose(handle);
    read_history(dir_history);
    stifle_history(500);
    write_history(dir_history);


    //ajout des variables d'environement dans notre shell
    int increment = 0;
    while (arge[increment] != NULL) {
        char *valeur = strstr(arge[increment], "=");
        char *nom = strndup(arge[increment], strlen(arge[increment]) - strlen(valeur));
        ajout_environnement(nom, valeur + 1);
        free(nom);
        ++increment;
    }
    ajout_environnement("?", "0");
    gethostname(hostname, TAILLE_BUFFER);
    ajout_environnement("HOSTNAME", hostname);




    //bind key clavier pour afichage des commandes
    //rl_bind_keyseq("\e[A", touche_fleche_haute);
    rl_bind_key('\t', touche_tab);
    rl_bind_keyseq("\t\t", double_touche_tab);

    // test si le shell a été ouvert avec un fichier script en argument
    if (argc > 1) {
        fichier = fopen(argv[1], "r");
        if (fichier == NULL) {
            fprintf(stderr, "Erreur ouverture %s", argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        fichier = stdin;
    }

    //boucle d'interaction
    while (1) {

        redirection_err = 0;
        char *ligne_saisie = lecture();
        if (ligne_saisie != NULL) {
            strcpy(buffer, ligne_saisie);
            free(ligne_saisie);

            if (isatty(fileno(fichier))) {
                add_history(buffer);
                append_history(1, dir_history);
            }
            traitement_ligne(argv);
        } else {
            exit(EXIT_SUCCESS);
        }
    }
}
