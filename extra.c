#include "extra.h"

Alias *exist_alias(char *nom_variable);


//parcour la list des VE et ajoute ou modifier la variable VE
void add_environnement(char *nom_variable, char *valeur_variable) {
    Environnement *liste = var_environnement;
    int test = 0;
    if (strncmp(valeur_variable, "$", 1) == 0) {
        valeur_variable = strdup(get_ve_value(valeur_variable + 1));
    }
    if (liste != NULL) {
        while (liste->next != NULL) {
            if (strcmp(nom_variable, liste->nom) == 0) {
                free(liste->valeur);
                liste->valeur = strdup(valeur_variable);
                test = 1;
                setenv(liste->nom, liste->valeur, 1);
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
        setenv(new_env->nom, new_env->valeur, 1);


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


//methode qui donne la valeur d'une variable e.
char *get_ve_value(char *nom_variable) {
    Environnement *liste = var_environnement;
    if (liste != NULL) {
        while (liste->next != NULL) {
            if (strcmp(nom_variable, liste->nom) == 0) {
                return liste->valeur;
            }
            liste = liste->next;
        }
    }
    return nom_variable;
}

void ajout_alias(char *nom_variable, char *valeur_variable) {
    Alias *liste = alias_list;
    Alias *tmp;

    //si alias exite alors modifier la valeur
    tmp = exist_alias(nom_variable);
    if (tmp != NULL) {
        free(tmp->valeur);
        tmp->valeur = strdup(valeur_variable);
    }
        //cas list vide ou pas trouver dans la liste
    else {
        Alias *new_alias = malloc(sizeof(Alias));
        new_alias->nom = strdup(nom_variable);
        new_alias->valeur = strdup(valeur_variable);
        new_alias->next = NULL;

        if (liste != NULL) {

            //parcourir la list si non vide sinon retourné l'element
            while (liste->next != NULL) {
                liste = liste->next;
            }
            liste->next = new_alias;
        } else {
            alias_list = new_alias;
        }
    }
}

//methode qui donne la valeur d'une variable e.
char *get_alias_value(char *nom_variable) {
    Alias *liste = alias_list;

    if (liste != NULL) {
        while (liste->next != NULL) {
            if (strcmp(nom_variable, liste->nom) == 0) {
                return liste->valeur;
            }
            liste = liste->next;
        }
    }
    return nom_variable;
}


//methode pour test si un alias exist deja
Alias *exist_alias(char *nom_variable) {
    Alias *liste = alias_list;
    while (liste != NULL) {
        if (strcmp(nom_variable, liste->nom) == 0) {
            return liste;
        }
        liste = liste->next;
    }
    return NULL;
}


//gestion des variable d'environement
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