#include "parser.h"


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


void traitement_espaces_debut(char *chaine_a_traiter) {
    char *nouvelle_chaine = chaine_a_traiter;
    while (nouvelle_chaine[0] == ' ') {
        ++nouvelle_chaine;
    }
    memmove(chaine_a_traiter, nouvelle_chaine, strlen(nouvelle_chaine) + 1);
}

void traitement_espaces_fin(char *chaine_a_traiter) {
    while (chaine_a_traiter[strlen(chaine_a_traiter) - 1] == ' ') {
        chaine_a_traiter[strlen(chaine_a_traiter) - 1] = '\0';
    }
}