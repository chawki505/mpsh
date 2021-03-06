#include "variables.h"


//methode de l'ecture du fichier mpshrc
void lecture_mpshrc() {
    char *line = NULL;
    FILE *file = fopen(dir_mpshrc, "r");
    if (file == NULL) {

        fprintf(stderr, "Erreur ouverture fichier de configuration .mpshrc\n");
        exit(EXIT_FAILURE);
    }

    line = malloc(TAILLE_BUFFER);


    while (1) {
        if (fgets(line, TAILLE_BUFFER, file) == NULL) break;

        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        supp_espaces_debut(line);
        sup_espaces_fin(line);

        if (line[0] == '#') {}
        else if (strncmp(line, "alias", 5) == 0) {
            //traitement de la ligne
            char *var = strdup(line);
            var = strstr(var, " ");
            if (var != NULL) {
                char *valeur_variable = strstr(var, "=");
                if (valeur_variable != NULL) {
                    char *nom_var = strndup(var + 1, strlen(var + 1) - strlen(valeur_variable));
                    ajouter_alias(nom_var, valeur_variable + 1);
                    free(nom_var);
                }
            } else {
                Alias *liste = alias_list;
                while (liste != NULL) {
                    printf("Alias : %s=%s\n", liste->nom, liste->valeur);
                    liste = liste->next;
                }
            }

        } else {
            //traitement de la variable de la ligne
            char *var = strdup(line);
            char *valeur_var = strstr(var, "=");

            if (valeur_var != NULL) {
                char *nom_var = strndup(var, strlen(var) - strlen(valeur_var));
                //TODO:traitement si il existe des var dans la valeur de var
                ajouter_variable_environnement(nom_var, valeur_var + 1);
                free(nom_var);
            } else {
            }
            free(var);
        }
    }

}

void init_ve(char **arge) {

    HOME = getenv("HOME");
    USER = getenv("USER");

    gethostname(HOSTNAME, TAILLE_BUFFER);

    dir_history[0] = '\0';
    strcat(dir_history, HOME);
    strcat(dir_history, "/.mpsh_history");

    dir_mpshrc[0] = '\0';
    strcat(dir_mpshrc, HOME);
    strcat(dir_mpshrc, "/.mpshrc");


    //ajout des variables d'environement dans notre shell
    int increment = 0;
    while (arge[increment] != NULL) {
        char *valeur = strstr(arge[increment], "=");
        char *nom = strndup(arge[increment], strlen(arge[increment]) - strlen(valeur));
        ajouter_variable_environnement(nom, valeur + 1);
        free(nom);
        ++increment;
    }
    ajouter_variable_environnement("?", "0");
    ajouter_variable_environnement("HOSTNAME", HOSTNAME);
    lecture_mpshrc();
    CHEMIN = getenv("CHEMIN");
}


//methode de l'ecture de l'entré du shell
char *lecture() {
    char *tmp = NULL;
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

    return tmp;
}


int main(int argc, char *argv[], char *arge[]) {
    //save le nombre d'arguments
    global_argc = argc;

    init_ve(arge);

    //init hystory
    using_history();

    //Si le fichier n'existe pas, l'historique ne pourra pas être lu et sauvegardé correctement.
    // Je teste donc la présence de celui-ci en l'ouvrant.
    // Si l'ouverture échoue, je crée le fichier avec fopen.
    FILE *handle = fopen(dir_history, "r");
    if (handle == NULL) handle = fopen(dir_history, "w");
    fclose(handle);
    read_history(dir_history);
    stifle_history(1000);
    write_history(dir_history);



    //bind key clavier pour afichage des commandes
    //rl_bind_keyseq("\e[A", touche_fleche_haute);
    rl_bind_key('\t', touche_tab);


    fichier = stdin;

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
