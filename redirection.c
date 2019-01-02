
#include "redirection.h"

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
