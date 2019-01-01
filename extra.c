#include "extra.h"


//parcour la list des VE et ajoute ou modifier la variable
void ajout_environnement(char *nom_variable, char *valeur_variable) {
    Environnement *liste = var_environnement;
    int test = 0;

    if (liste != NULL) {
        while (liste->next != NULL) {
            if (strcmp(nom_variable, liste->nom) == 0) {
                free(liste->valeur);
                liste->valeur = strdup(valeur_variable);
                test = 1;
            }
            liste = liste->next;
        }
    }

    //cas VR non trouver
    if (test == 0) {
        Environnement *new_env = malloc(sizeof(Environnement));

        new_env->nom = strdup(nom_variable);
        new_env->valeur = strdup(valeur_variable);
        new_env->next = NULL;

        liste = var_environnement;


        if (liste != NULL) {

            while (liste->next != NULL) {
                liste = liste->next;
            }
            liste->next = new_env;

        } else {
            var_environnement = new_env;
        }
    }
}

void ajout_alias(char *nom_variable, char *valeur_variable) {
    Environnement *liste = var_environnement;
    int test = 0;

    if (liste != NULL) {
        while (liste->next != NULL) {
            if (strcmp(nom_variable, liste->nom) == 0) {
                free(liste->valeur);
                liste->valeur = strdup(valeur_variable);
                test = 1;
            }
            liste = liste->next;
        }
    }

    if (test == 0) {
        Environnement *new_env = malloc(sizeof(Environnement));

        new_env->nom = strdup(nom_variable);
        new_env->valeur = strdup(valeur_variable);
        new_env->next = NULL;

        liste = var_environnement;


        if (liste != NULL) {
            while (liste->next != NULL) {
                liste = liste->next;
            }
            liste->next = new_env;
        } else {
            var_environnement = new_env;
        }
    }
}


//gestion des variable dans un fichier script.sh
void gestion_variables(char *arguments[TAILLE_LIST_ARGS], char **argv) {
    int increment = 0;

    while (arguments[increment] != NULL) {
        char *chaine_a_scanner = arguments[increment];

        if (chaine_a_scanner[0] == '$') {

            int detection = 0;
            char *endptr = NULL;
            long entier = strtol(arguments[increment] + 1, &endptr, 10);
            if (strcmp(endptr, "") != 0) entier = -1;

            if (entier != -1) {

                if (entier + 1 <= global_argc) {
                    free(arguments[increment]);
                    arguments[increment] = strdup(argv[entier]);
                    detection = 1;
                }

            } else {

                Environnement *liste = var_environnement;

                while (liste != NULL) {
                    if (strcmp(liste->nom, chaine_a_scanner + 1) == 0) {
                        free(arguments[increment]);
                        arguments[increment] = strdup(liste->valeur);
                        detection = 1;
                    }
                    liste = liste->next;
                }
            }

            if (detection == 0) {
                free(arguments[increment]);
                arguments[increment] = strdup("");
            }
        }

        ++increment;
    }
}


int touche_fleche_haute() {
    HIST_ENTRY *historique = history_get(history_length);
    rl_replace_line(historique->line, 0);
    rl_end_of_line(0, 0);
    return 0;
}

int touche_tab() {
    char *buffer = strdup(rl_line_buffer);
    char *buffer_reallocation = realloc(buffer, strlen(buffer) + 2);
    if (buffer_reallocation == NULL) return 0; else buffer = buffer_reallocation;
    char *tmp = strstr(buffer, " ");
    if (tmp == NULL) tmp = buffer;
    if (tmp[0] == ' ') ++tmp;
    strcat(tmp, "*");
    glob_t g;
    int retour_glob = glob(tmp, 0, NULL, &g);
    if (retour_glob == 0) {
        if (g.gl_pathc == 1) {
            char *new_buffer = malloc(strlen(buffer) + strlen(g.gl_pathv[0]) + 2);
            new_buffer[0] = '\0';
            strncat(new_buffer, buffer, strlen(buffer) - strlen(tmp));
            strcat(new_buffer, g.gl_pathv[0]);
            rl_replace_line(new_buffer, 0);
            rl_end_of_line(0, 0);
            free(new_buffer);
        }
    }
    globfree(&g);
    free(buffer);
    return 0;
}

int double_touche_tab() {
    char *buffer = strdup(rl_line_buffer);
    char *buffer_reallocation = realloc(buffer, strlen(buffer) + 2);
    if (buffer_reallocation == NULL) return 0; else buffer = buffer_reallocation;
    char *tmp = strstr(buffer, " ");
    if (tmp == NULL) tmp = buffer;
    if (tmp[0] == ' ') ++tmp;
    strcat(tmp, "*");
    glob_t g;

    int retour_glob = glob(tmp, 0, NULL, &g);
    if (retour_glob == 0 && g.gl_pathc >= 1) {
        char *new_buffer = malloc(strlen(buffer) + 3 + 2);
        new_buffer[0] = '\0';
        strcat(new_buffer, "ls ");
        strcat(new_buffer, tmp);

        system(new_buffer);

        rl_on_new_line();
        free(new_buffer);
    }
    globfree(&g);
    free(buffer);
    return 0;
}
