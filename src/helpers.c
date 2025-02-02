#include "header.h"

/* 
 Parcourt le dossier donné et copie dans un tableau statique les noms des fichiers
 se terminant par ".txt". Le nombre d'éléments est renvoyé via *nb.
 En cas d'erreur, la structure Erreur est renseignée et la fonction retourne NULL.
 */
char (*LectureNoms(char *dossierChemin, int *nb, Erreur *err))[MAX_NAME_LEN] {
    static char noms_static[MAX_NIVEAUX][MAX_NAME_LEN];
    *nb = 0;

    DIR *dossier = opendir(dossierChemin);
    if (dossier == NULL) {
        if (err) {
            snprintf(err->msg_erreur, sizeof(err->msg_erreur),
                     "Impossible d'ouvrir le dossier %s.", dossierChemin);
            err->statut_erreur = 1;
        }
        *nb = 0;
        return NULL;
    }

    struct dirent *ent;
    while ((ent = readdir(dossier)) != NULL && (*nb) < MAX_NIVEAUX) {
        if (ent->d_type == DT_REG) {
            int len = strlen(ent->d_name);
            if (len > 4 && strcmp(ent->d_name + len - 4, ".txt") == 0) {
                strncpy(noms_static[*nb], ent->d_name, MAX_NAME_LEN - 1);
                noms_static[*nb][MAX_NAME_LEN - 1] = '\0';
                (*nb)++;
            }
        }
    }
    closedir(dossier);

    if (*nb == 0) {
        if (err) {
            snprintf(err->msg_erreur, sizeof(err->msg_erreur),
                     "Aucun fichier .txt trouvé dans le dossier %s.", dossierChemin);
            err->statut_erreur = 1;
        }
        return NULL;
    }
    return noms_static;
}

/*
 Cette fonction formate un tableau statique de noms de fichiers pour l'affichage.
 Elle trie d'abord les noms (tri à bulles basé sur le numéro extrait s'ils commencent par un chiffre)
 puis, pour chaque nom, retire l'extension ".txt" et remplace les underscores par des espaces.
 Le résultat est stocké dans un tableau statique et renvoyé.
 */
char (*FormatterNoms(char noms[][MAX_NAME_LEN], int nb, Erreur *err))[MAX_NAME_LEN] {
    static char options[MAX_NIVEAUX][MAX_NAME_LEN];
    int i, j;

    /* Tri à bulles sur 'noms' selon le numéro extrait (si présent) */
    for (i = 0; i < nb - 1; i++) {
        for (j = i + 1; j < nb; j++) {
            int num_i = -1, num_j = -1;
            if (noms[i][0] >= '0' && noms[i][0] <= '9')
                sscanf(noms[i], "%d", &num_i);
            if (noms[j][0] >= '0' && noms[j][0] <= '9')
                sscanf(noms[j], "%d", &num_j);
            if (num_i > num_j) {
                char temp[MAX_NAME_LEN];
                strncpy(temp, noms[i], MAX_NAME_LEN);
                strncpy(noms[i], noms[j], MAX_NAME_LEN);
                strncpy(noms[j], temp, MAX_NAME_LEN);
            }
        }
    }

    /* Formatage de chaque nom pour l'affichage */
    for (i = 0; i < nb; i++) {
        char formatted[250];  // Buffer temporaire pour le nom formaté
        if (noms[i][0] >= '0' && noms[i][0] <= '9') {
            int numero;
            char texte[200];
            if (sscanf(noms[i], "%d_%199[^.].txt", &numero, texte) == 2) {
                for (int k = 0; texte[k] != '\0'; k++) {
                    if (texte[k] == '_')
                        texte[k] = ' ';
                }
                snprintf(formatted, sizeof(formatted), "Niveau %d : %s", numero, texte);
            } else {
                char temp[200];
                if (sscanf(noms[i], "%199[^.].txt", temp) == 1)
                    snprintf(formatted, sizeof(formatted), "Niveau : %s", temp);
                else
                    snprintf(formatted, sizeof(formatted), "%s", noms[i]);
            }
        } else {
            char temp[200];
            if (sscanf(noms[i], "%199[^.].txt", temp) == 1) {
                for (int k = 0; temp[k] != '\0'; k++) {
                    if (temp[k] == '_')
                        temp[k] = ' ';
                }
                snprintf(formatted, sizeof(formatted), "Exterieur : %s", temp);
            } else {
                snprintf(formatted, sizeof(formatted), "%s", noms[i]);
            }
        }
        strncpy(options[i], formatted, MAX_NAME_LEN - 1);
        options[i][MAX_NAME_LEN - 1] = '\0';
    }
    return options;
}

/*
 Permet à l'utilisateur de choisir un leaderboard à afficher.
 1) Récupère les noms des fichiers dans le dossier "data_leaderboard" via LectureNoms.
 2) Formate ces noms via FormatterNoms.
 3) Ajoute une option "Retour au menu".
 4) Affiche le menu de choix et, si un leaderboard est sélectionné, appelle AfficherLeaderboard.
 */
void ChoixLeaderboard(Erreur *err) {
    int nb = 0;
    char (*noms)[MAX_NAME_LEN] = LectureNoms("data_leaderboard", &nb, err);
    if (noms == NULL || nb == 0) {
        printf("Aucun leaderboard disponible.\n");
        sleep(1);
        return;
    }

    char (*options)[MAX_NAME_LEN] = FormatterNoms(noms, nb, err);

    /* Ajout de l'option "Retour au menu" */
    strncpy(options[nb], "Retour au menu", MAX_NAME_LEN - 1);
    options[nb][MAX_NAME_LEN - 1] = '\0';
    int nbOptions = nb + 1;

    /* Pour chaque option, retirer le suffixe " leaderboard" s'il existe */
    for (int i = 0; i < nbOptions; i++) {
        char *pos = strstr(options[i], " leaderboard");
        if (pos)
            *pos = '\0';
    }

    printf("\t\t\t\t\t\t\t\033[36m▲▼▲▼▲ CHOIX DU LEADERBOARD ▲▼▲▼▲\033[0m\n\n");

    int choix = AfficherChoix(options, nbOptions, err);
    if (choix == nbOptions - 1)
        return;
    if (choix >= 0 && choix < nbOptions - 1) {
        AfficherLeaderboard(noms[choix], err);
    }
}

/*
 Récupère le nom de fichier à partir d'un chemin complet.
 La fonction cherche le dernier '/' et copie la sous-chaîne suivante dans un buffer statique.
 Ensuite, elle retire l'extension (la dernière occurrence de '.').
 Cette fonction n'utilise pas d'allocation dynamique.
 */
char* RecupererNom(const char* chemin) {
    static char copie[MAX_NAME_LEN];
    const char* nom = strrchr(chemin, '/');
    if (!nom) {
        strncpy(copie, chemin, MAX_NAME_LEN - 1);
        copie[MAX_NAME_LEN - 1] = '\0';
    } else {
        nom++; // passe le '/'
        strncpy(copie, nom, MAX_NAME_LEN - 1);
        copie[MAX_NAME_LEN - 1] = '\0';
    }
    char* point = strrchr(copie, '.');
    if (point)
        *point = '\0';
    return copie;
}
