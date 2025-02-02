#include "header.h"

void ResoudreFichier(FILE* fichier_ennemis, Erreur* erreur) {
    if (fichier_ennemis == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "fichier invalide\n");
        return;
    }

    // tableau alloué non dynamiquement, pas besoin de free
    
    int ligne_courante = 0;
    int tour_courant = 0;

    char ligne_fichier[256];
    // boucle pour lire les lignes du fichier 
    while (fgets(ligne_fichier, sizeof(ligne_fichier), fichier_ennemis)) {
        int tour, ligne;
        char type;
        if (sscanf(ligne_fichier, "%d %d %c", &tour, &ligne, &type) != 3) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "format de ligne invalide\n");
            return;
        }

        if (ligne < 1 || ligne > NB_LIGNES || tour < 0){
            erreur->statut_erreur = 1;
            sprintf(erreur->msg_erreur, "le tour ou la ligne entré(e) est invalide : %d\n", ligne);
            return;
        }
        if (tour < tour_courant) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "l'apparition des ennemis n'est pas triée par tour\n");
            return;
        }
        if (tour == tour_courant && ligne <= ligne_courante) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "l'apparition des ennemis n'est pas triée par ligne\n");
            return;
        }
        tour_courant = tour;
        ligne_courante = ligne;
    }

    // remet le curseur au début du fichier
    rewind(fichier_ennemis);
    fgets(ligne_fichier, sizeof(ligne_fichier), fichier_ennemis);  // skip une ligne (la cagnotte)
    printf("Le fichier est trié\n");
}


/* 
 Parcourt le dossier donné et copie dans un tableau statique les noms des fichiers
 se terminant par ".txt". Le nombre d'éléments est renvoyé via *nb.
 En cas d'erreur la fonction retourne NULL.
 */
char (*LectureNoms(char *dossierChemin, int *nb, Erreur *err))[MAX_NAME_LEN] {
    // static pour que les tableaux soient conservés entre les appels
    static char noms[MAX_NIVEAUX][MAX_NAME_LEN];
    *nb = 0;

    DIR *dossier = opendir(dossierChemin);
    if (!dossier) {
        err->statut_erreur = 1;
        snprintf(err->msg_erreur, sizeof(err->msg_erreur), "Impossible d'ouvrir le dossier %s", dossierChemin);
        return NULL;
    }

    struct dirent *ent;


    // Parcourt les fichiers .txt (ignore . et .. et ne se soucie pas du type)
    while ((ent = readdir(dossier)) && *nb < MAX_NIVEAUX) {
        // Ignore '.' et '..'
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        // longueur du nom de fichier
        int len = strlen(ent->d_name);
        if (len > 4 && strcmp(ent->d_name + len - 4, ".txt") == 0) {
            strncpy(noms[*nb], ent->d_name, MAX_NAME_LEN - 1);
            noms[*nb][MAX_NAME_LEN - 1] = '\0';
            (*nb)++;

        }
    }
    closedir(dossier);
    if (*nb == 0) {
        err->statut_erreur = 1;
        strcpy(err->msg_erreur, "Aucun fichier .txt trouvé dans le dossier");
        return NULL;
    }
    return noms;
}


int comparer_niveaux(const void* a, const void* b) {
    char* nom1 = *(char**)a;
    char* nom2 = *(char**)b;
    
    int num1, num2;
    sscanf(nom1, "%d", &num1);
    sscanf(nom2, "%d", &num2);
    
    return num1 - num2;
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


// Cette fonction permet à l'utilisateur de choisir un leaderboard à afficher.
// 1) On ouvre le dossier "data_leaderboard" qui contient tous les fichiers texte avec les classements pour chaque partie jouée. (via LectureNoms)
// 2) FormatterNoms créer des options plus lisibles qui seront affichées comme choix grâce à AfficherChoix.
// 3) On renvoie le choix tapé au clavier par l'utilisateur pour ouvrir le fichier en question !

void ChoixLeaderboard(Erreur *erreur) {
    int nb = 0;
    char (*noms)[MAX_NAME_LEN] = LectureNoms("data_leaderboard", &nb, erreur);
    if (noms == NULL || nb == 0) {
        printf("Aucun leaderboard disponible.\n");
        sleep(1);
        return;
    }
    // Trier les noms de fichiers et enlever les extensions pour l'affichage
    char (*options)[MAX_NAME_LEN] = FormatterNoms(noms, nb, erreur);

    // Ajout de l'option "Retour au menu" 
    strcpy(options[nb], "Retour au menu");
    int nbOptions = nb + 1;

    // Pour chaque option, retirer le suffixe " leaderboard" s'il existe 
    for (int i = 0; i < nbOptions; i++) {
        char *pos = strstr(options[i], " leaderboard");
        if (pos)
            *pos = '\0';
    }

    // Affichage graphique des choix + on récupère l'option choisie.
    printf("\t\t\t\t\t\t\t\033[36m▲▼▲▼▲ CHOIX DU LEADERBOARD ▲▼▲▼▲\033[0m\n\n");

    int choix = AfficherChoix(options, nbOptions, erreur);
    // Si l'utilisateur choisit "Retour au menu"
    if (choix == nbOptions - 1) {
        return;
    }
    
    // Si un leaderboard a été choisi, on initialise une structure de jeu pour simuler un affichage de leaderboard de fin de partie.
    if (choix >= 0 && choix < nbOptions - 1 && noms[choix]) {
        AfficherLeaderboard(noms[choix], erreur);
    }
}

// Récupère le nom de fichier à partir d'un chemin complet en utilisant un buffer statique ; 
// retire l'extension (la dernière occurrence de '.') ; 
// en cas de problème (résultat vide), renseigne la structure Erreur.
char* RecupererNom(const char* chemin, Erreur *err) {
    static char copie[MAX_NAME_LEN]; // Buffer statique pour le nom extrait
    const char* nom = strrchr(chemin, '/'); // Cherche le dernier '/' dans le chemin
    if (!nom) { // Si aucun '/' n'est trouvé, copie le chemin entier
        strncpy(copie, chemin, MAX_NAME_LEN - 1);
        copie[MAX_NAME_LEN - 1] = '\0';
    } else {
        nom++; // Passe le '/'
        strncpy(copie, nom, MAX_NAME_LEN - 1);
        copie[MAX_NAME_LEN - 1] = '\0';
    }
    char* point = strrchr(copie, '.'); // Cherche le dernier '.' pour l'extension
    if (point)
        *point = '\0'; // Retire l'extension
    if (strlen(copie) == 0 && err) { // Si le résultat est vide, renseigne la structure Erreur
        snprintf(err->msg_erreur, sizeof(err->msg_erreur), "Nom de fichier vide après extraction");
        err->statut_erreur = 1;
    }
    return copie;
}

// fait changer l'ennemi de ligne au cours du jeu, ne fonctionne pas
void ChangerLigne(Jeu * jeu, Etudiant* e, int saut) {
    printf("L'ennemi est à la position %d\n", e->position);
    if (e == NULL || jeu == NULL || saut == 0) {
        return;
    }
    // change la ligne de l'ennemi
    e->ligne += saut;
    // modifie le chaînage de la ligne d'origine
    if (e->prev_line) {
        e->prev_line->next_line = e->next_line;
    }
    if (e->next_line) {
        e->next_line->prev_line = e->prev_line;
    }
    e->prev_line = NULL;
    e->next_line = NULL;
    //place l'ennemi 
    Etudiant * current = jeu->etudiants;
    while (current) {
        if (current->ligne == e->ligne) {
            break;
        }
        current = current->next;
    }
    printf("current est à la position %d\n", current->position);
    printf("current est à la ligne %d\n", current->ligne);
    printf("current est de type %c\n", (char)current->type);
    if (!current) {
        printf("il est seul sur la ligne %d\n", e->ligne);
        printf(" est à la position %d\n", e->position);
        return;
    }

    Etudiant * devant = NULL;
    while (current && current->position < e->position) {
        printf("prochain ennemi : %d\n", current->position);
        devant = current;
        current = current->next_line;
    } 
    // - 'prev_ennemi' pointe vers le 1er ennemi ayant une position >= e->position
    //   OU bien prev_ennemi est NULL (fin de liste).
    // - 'devant' pointe vers l'ennemi qui a la position immédiatement derrière e (plus grande que e->position),
    //   OU  NULL si e doit s’insérer au tout début.

    // prev_ennemi est maintenant le l'ennemi le plus proche derrière
    // si les positions sont identiques, on décale de 1 tant que la position est occupée

    printf("current est à la position %d\n", current->position);
    if (devant) {
        printf("devant est à la position %d\n", devant->position);
    }
    while (current && current != e && current->position == e->position && current->pointsDeVie >0) {
        printf("incrémentation de la position \n");
        printf("prochaine ennemi type : %c\n", (char)current->type);
        e->position++;
        devant = current;
        current = current->next_line;
    }

    e->prev_line = devant;
    e->next_line = current;


    if (devant) {
        devant->next_line = e;
    }
    if (current) {
        current->prev_line = e;
    }
    printf("L'ennemi a sauté sur la ligne %d\n", e->ligne);
    printf("L'ennemi est à la position %d\n", e->position);
}