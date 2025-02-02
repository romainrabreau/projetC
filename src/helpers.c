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

char** LectureNoms(DIR* dossier) {
    // static pour que les tableaux soient conservés entre les appels
    static char nomsFichiers[100][256];  // tableau de noms de fichiers
    static char * pointeursFichiers[100];  // tableau de pointeurs vers les noms de fichiers
    int count = 0;

    struct dirent* ent;
    rewinddir(dossier); // S'assurer de lire depuis le début

    while ((ent = readdir(dossier)) != NULL && count < 100) {
        // Vérifier si c'est un fichier .txt
        if (ent->d_type == DT_REG) { // Fichier régulier
            const char* nom = ent->d_name;
            size_t len = strlen(nom);

            if (len > 4 && strcmp(nom + len - 4, ".txt") == 0) {
                strncpy(nomsFichiers[count], nom, 255);
                nomsFichiers[count][255] = '\0';
                pointeursFichiers[count] = nomsFichiers[count];
                count++;
            }
        }
    }
    pointeursFichiers[count] = NULL;
    return pointeursFichiers;
}


int comparer_niveaux(const void* a, const void* b) {
    char* nom1 = *(char**)a;
    char* nom2 = *(char**)b;
    
    int num1, num2;
    sscanf(nom1, "%d", &num1);
    sscanf(nom2, "%d", &num2);
    
    return num1 - num2;
}


// Cette fonction formate un tableau de noms de niveaux pour l'affichage d'options essentiellement.
// Si un fichier commence par un chiffre, il est considéré comme "Niveau <numéro> : <nom>".
// Sinon, il est considéré comme un niveau extérieur et est affiché sous la forme "Exterieur : <nom>".
// Chaque nom est traité pour retirer l'extension ".txt" et remplacer les underscores par des espaces.
// Extension possible : considérer plus que le premier caractère comme niveau.
// Extension possible : merge les deux cas avec un numéro ou pas dans une fonction car le processus de parsing est le même.
char** FormatterNoms(char** noms) {
    if (noms == NULL)
        return NULL;
    
    // Calcule le nombre d'éléments dans le tableau
    int count = 0;
    while (noms[count] != NULL)
        count++;
    
    // Trie le tableau de noms à l'aide de qsort pour mettre les niveaux dans l'odre (ils commencent par un chiffre entre 1 et 9)
    qsort(noms, count, sizeof(char*), comparer_niveaux);
    
    // On créer nouveau tableau pour stocker résultats avec malloc car on ne connait pas à l'avance la taille du tableau.
    char** resultats = malloc((count + 1) * sizeof(char*));
    if (resultats == NULL)
        return NULL;
    
    // Traitement des noms 
    for (int i = 0; i < count; i++) {
        char buffer[250];  // Buffer temporaire pour le nom en cours de formattage
        
        // Si le premier caractère est un chiffre on considère que c'est un nievau.
        if (noms[i][0] >= '0' && noms[i][0] <= '9') {
            int numero;
            char nomTemp[200];
            if (sscanf(noms[i], "%d_%199[^.].txt", &numero, nomTemp) == 2) {
                // On remplace chaque underscore dans le nom extrait par un espace
                for (char* p = nomTemp; *p != '\0'; p++) {
                    if (*p == '_')
                        *p = ' ';
                }
                // Le format du nom en sortie est "Niveau <numéro> : <nom>".
                snprintf(buffer, sizeof(buffer), "Niveau %d : %s", numero, nomTemp);
            } else {
                // En cas d'échec, on utilise le nom original
                snprintf(buffer, sizeof(buffer), "%s", noms[i]);
            }
        }
        // Sinon, le nom ne commence pas par un chiffre, on le considère comme 'Extérieur', Cad un niveau charger par l'utilisateur en général.
        else {
            char nomTemp[200];
            if (sscanf(noms[i], "%199[^.].txt", nomTemp) == 1) {
                for (char* p = nomTemp; *p != '\0'; p++) {
                    if (*p == '_')
                        *p = ' ';
                }
                // Le formate du nom en sotrie est "Exterieur : <nom>"
                snprintf(buffer, sizeof(buffer), "Exterieur : %s", nomTemp);
            } else {
                // En cas d'échec du parsing, on utilise le nom original
                snprintf(buffer, sizeof(buffer), "%s", noms[i]);
            }
        }
        // Duplique le contenu du buffer et le stocke dans le tableau des résultats
        resultats[i] = strdup(buffer);
    }
    // Termine le tableau par un pointeur NULL pour indiquer qu'il n'y a plus d'éléments à traiter
    resultats[count] = NULL;
    
    return resultats;
}



// Cette fonction permet à l'utilisateur de choisir un leaderboard à afficher.
// 1) On ouvre le dossier "data_leaderboard" qui contient tous les fichiers texte avec les classements pour chaque partie jouée. (via LectureNoms)
// 2) FormatterNoms créer des options plus lisibles qui seront affichées comme choix grâce à AfficherChoix.
// 3) On renvoie le choix tapé au clavier par l'utilisateur pour ouvrir le fichier en question !

void ChoixLeaderboard(Erreur* erreur) {
    // 1) On ouvre le dossier "data_leaderboard"
    DIR *dossier = opendir("data_leaderboard");
    if (!dossier) { 
        printf("Aucun niveau trouvé.\n"); 
        sleep(1); 
        return; 
    }
    
    char noms[100][256];
    char *options[101];
    int count = 0;

    struct dirent *entry;
    // on parcourt le dossier pour récupérer les fichiers leaderboard
    while ((entry = readdir(dossier)) != NULL && count < 100) {
        if (strstr(entry->d_name, "leaderboard")) {
            strncpy(noms[count], entry->d_name, 255);
            noms[count][255] = '\0';
            options[count] = noms[count]; 
            count++;
        }
    }
    closedir(dossier);
    options[count] = "Retour au menu";
    options[count + 1] = NULL;

    // Pour chaque option, on retire le suffixe " leaderboard" car tous les fichiers du repertoire l'ont.
    for (int i = 0; i < count -1; i++) {
        char *pos = strstr(options[i], " leaderboard");
        if (pos)
            *pos = '\0';
    }
    // Affichage graphique des choix + on récupère l'option choisie.
    printf("\t\t\t\t\t\t\t\033[36m▲▼▲▼▲ CHOIX DU LEADERBOARD ▲▼▲▼▲\033[0m\n\n");

    char **formatted = FormatterNoms(options);
    int choix = AfficherChoix(options, count + 1);  
    
    // Si l'utilisateur choisit "Retour au menu"
    if (choix == count - 1) {
        return;
    }
    
    // Si un leaderboard a été choisi, on initialise une structure de jeu pour simuler un affichage de leaderboard de fin de partie.
    if (choix >= 0 && choix < count - 1 && noms[choix]) {
        AfficherLeaderboard(noms[choix], erreur);
    }
}

void LibererNomsFormates(char** noms) {
    if (!noms) return;
    for (int i = 0; noms[i]; i++) free(noms[i]);
    free(noms);
}

char* RecupererNom(const char* chemin) {
    // Cherche le dernier '/' et prend la sous-chaîne suivante
    const char* nom = strrchr(chemin, '/');
    if (!nom) {
        nom = chemin;
    } else {
        nom++; // passe le '/'
    }
    // Duplique la chaîne
    char* copie = strdup(nom);
    if (!copie) return NULL;
    // Supprime l'extension, si présente (on cherche le dernier '.')
    char* dot = strrchr(copie, '.');
    if (dot) {
        *dot = '\0';
    }
    return copie;
}

// ne fonctionne pas
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