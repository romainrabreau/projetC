#include "header.h"
// fonction de tri pour qsort
int comparerEnnemis(const void* a, const void* b) {
    const Etudiant* ennemiA = (const Etudiant*)a;
    const Etudiant* ennemiB = (const Etudiant*)b;
    if (ennemiA->tour != ennemiB->tour) {
        return ennemiA->tour - ennemiB->tour;
    }
    return ennemiA->ligne - ennemiB->ligne;
}

void ResoudreFichier(FILE* fichier_ennemis, Erreur* erreur) {
    if (fichier_ennemis == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "fichier invalide\n");
        return;
    }

    int cagnotte = 0; // la cagnotte n'était pas lue donc ça buguait
    char newline;
    if (fscanf(fichier_ennemis, "%d%c", &cagnotte, &newline) != 2 || newline != '\n') {
        // Erreur si on ne lit pas un entier suivi d’un saut de ligne
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Cagnotte introuvable ou invalide\n");
        return;
    }

    // tableau alloué non dynamiquement, pas besoin de free
    Etudiant ennemis[NB_ENNEMIS_MAX];
    int nbEnnemis = 0;
    char ligne_fichier[256];

    while (fgets(ligne_fichier, sizeof(ligne_fichier), fichier_ennemis)) {
        int tour, ligne;
        char type;
        if (sscanf(ligne_fichier, "%d %d %c", &tour, &ligne, &type) != 3) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "format de ligne invalide\n");
            fclose(fichier_ennemis);
            return;
        }
        if (tour < 0 || ligne < 1 || ligne > NB_LIGNES) {
            erreur->statut_erreur = 1;
            sprintf(erreur->msg_erreur,
                    "Tour/ligne invalide (tour=%d, ligne=%d)\n", tour, ligne);
            fclose(fichier_ennemis);
            return;
        }
        ennemis[nbEnnemis].tour  = tour;
        ennemis[nbEnnemis].ligne = ligne;
        ennemis[nbEnnemis].type  = type;
        nbEnnemis++;
        if (nbEnnemis >= NB_ENNEMIS_MAX) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Trop d'ennemis\n");
            fclose(fichier_ennemis);
            return;
        }
    }

    // Tri initial
    qsort(ennemis, nbEnnemis, sizeof(Etudiant), comparerEnnemis);

    // Résolution des collisions : on incrémente le 'tour' de l'ennemi suivant s'il y a collision
    if (nbEnnemis > 1) {
        int collision;
        do {
            collision = 0;
            for (int i = 0; i < nbEnnemis - 1; i++) {
                for (int j = i + 1; j < nbEnnemis; j++) {
                    if (ennemis[i].tour == ennemis[j].tour &&
                        ennemis[i].ligne == ennemis[j].ligne)
                    {
                        ennemis[j].tour++;  // On repousse le tour
                        collision = 1;
                    }
                }
            }
            if (collision) {
                qsort(ennemis, nbEnnemis, sizeof(Etudiant), comparerEnnemis);
            }
        } while (collision);
    }

    // On réécrit depuis le début
    rewind(fichier_ennemis);
    for (int i = 0; i < nbEnnemis; i++) {
        fprintf(fichier_ennemis, "%d %d %c\n",
                ennemis[i].tour, ennemis[i].ligne, ennemis[i].type);
    }

    fflush(fichier_ennemis);
    fclose(fichier_ennemis);
}

void Attendre(int ms) {
    usleep(ms * 1000);  // Pause en millisecondes
}

char** LectureNoms(DIR* dossier) {
    struct dirent* ent;
    char** noms = NULL;
    int count = 0;

    rewinddir(dossier); // S'assurer de lire depuis le début

    while ((ent = readdir(dossier)) != NULL) {
        // Vérifier si c'est un fichier .txt
        if (ent->d_type == DT_REG) { // Fichier régulier
            const char* nom = ent->d_name;
            size_t len = strlen(nom);
            if (len > 4 && strcmp(nom + len - 4, ".txt") == 0) {
                // Allouer de la mémoire pour le nouveau nom
                char* nom_copie = strdup(nom);
                if (!nom_copie) {
                    // Échec de l'allocation, nettoyer et retourner NULL
                    for (int i = 0; i < count; i++) free(noms[i]);
                    free(noms);
                    return NULL;
                }

                // Étendre le tableau
                char** temp = realloc(noms, (count + 1) * sizeof(char*));
                if (!temp) {
                    free(nom_copie);
                    for (int i = 0; i < count; i++) free(noms[i]);
                    free(noms);
                    return NULL;
                }
                noms = temp;
                noms[count++] = nom_copie;
            }
        }
    }

    char** temp = realloc(noms, (count + 1) * sizeof(char*));
    if (!temp) {
        for (int i = 0; i < count; i++) free(noms[i]);
        free(noms);
        return NULL;
    }
    noms = temp;
    noms[count] = NULL;

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

char** FormatterNoms(char** noms) {
    if (!noms) return NULL;
    
    int count = 0;
    while (noms[count]) count++;
    
    qsort(noms, count, sizeof(char*), comparer_niveaux);
    
    char** resultats = malloc((count + 1) * sizeof(char*));
    if (!resultats) return NULL;
    
    for (int i = 0; i < count; i++) {
        char buffer[256];
        // Si le nom commence par un chiffre, on attend le format "nb_Nomniveau.txt"
        if (isdigit((unsigned char)noms[i][0])) {
            int numero;
            char nom[200];
            if (sscanf(noms[i], "%d_%199[^.].txt", &numero, nom) == 2) {
                // Remplacer les underscores par des espaces
                for (char* p = nom; *p; p++) {
                    if (*p == '_')
                        *p = ' ';
                }
                snprintf(buffer, sizeof(buffer), "Niveau %d : %s", numero, nom);
            } else {
                // En cas d'échec du parsing, on utilise le nom brut
                snprintf(buffer, sizeof(buffer), "%s", noms[i]);
            }
        }
        // Sinon, le nom ne suit pas le format nb_Nomniveau, on le considère comme extérieur.
        else {
            char nom[200];
            if (sscanf(noms[i], "%199[^.].txt", nom) == 1) {
                for (char* p = nom; *p; p++) {
                    if (*p == '_')
                        *p = ' ';
                }
                snprintf(buffer, sizeof(buffer), "Exterieur : %s", nom);
            } else {
                snprintf(buffer, sizeof(buffer), "%s", noms[i]);
            }
        }
        resultats[i] = strdup(buffer);
    }
    resultats[count] = NULL;
    
    return resultats;
}

void LibererNomsFormates(char** noms) {
    if (!noms) return;
    for (int i = 0; noms[i]; i++) free(noms[i]);
    free(noms);
}

// Cette fonction permet à l'utilisateur de choisir un leaderboard à afficher.
// 1) On ouvre le dossier "data_leaderboard" qui contient tous les fichiers texte avec les classements pour chaque partie jouée. (via LectureNoms)
// 2) FormatterNoms créer des options plus lisibles qui seront affichées comme choix grâce à AfficherChoix.
// 3) On renvoie le choix tapé au clavier par l'utilisateur pour ouvrir le fichier en question !
void ChoixLeaderboard() {
    // 1) On ouvre le dossier "data_leaderboard"
    DIR *dossier = opendir("data_leaderboard");
    if (!dossier) { 
        printf("Aucun niveau trouvé.\n"); 
        sleep(1); 
        return; 
    }
    
    // Et on en extrait son contenu.
    char **noms = LectureNoms(dossier);
    closedir(dossier);
    if (!noms || !noms[0]) {
        if (noms) { 
            for (int i = 0; noms[i]; i++) 
                free(noms[i]); 
            free(noms); 
        }
        printf("Aucun leaderboard disponible.\n");
        sleep(1);
        return;
    }

    // 2) On formate les noms lus pour les afficher en options de façon plus digeste
    char **options = FormatterNoms(noms);
    int count = 0;
    while (options[count])
        count++;

    // Ajout de l'option "Retour au menu" à la fin du tableau des options
    options = realloc(options, (count + 2) * sizeof(char *));
    options[count] = strdup("Retour au menu");
    options[count + 1] = NULL;
    count++;

    // Pour chaque option, on retire le suffixe " leaderboard" car tous les fichiers l'ont
    for (int i = 0; i < count; i++) {
        char *pos = strstr(options[i], " leaderboard");
        if (pos)
            *pos = '\0';
    }

    // Utilise une largeur de terminal par défaut (80 colonnes) pour calculer l'espacement
    int terminal_width = 80;
    int padding = (terminal_width - 50) / 2;
    if (padding < 0)
        padding = 0;

    // Affiche le titre du menu du leaderboard avec mise en forme ANSI et 7 tabulations simulées par padding
    printf("%*s\033[36m▲▼▲▼▲ CHOIX DU LEADERBOARD ▲▼▲▼▲\033[0m\n\n", padding - 5, "");
    int choix = AfficherChoix(options, count);
    
    // Si l'utilisateur choisit "Retour au menu"
    if (choix == count - 1) {
        LibererNomsFormates(options);
        for (int i = 0; noms[i]; i++) 
            free(noms[i]);
        free(noms);
        return;
    }
    
    // Si un leaderboard a été choisi, initialise une structure de jeu simple et affiche le leaderboard correspondant
    if (choix >= 0 && choix < count - 1 && noms[choix]) {
        Jeu jeu;
        memset(&jeu, 0, sizeof(Jeu));
        strncpy(jeu.fichier_ennemis, noms[choix], sizeof(jeu.fichier_ennemis) - 1);
        AfficherLeaderboard(&jeu);
    }
    
    // Libère la mémoire allouée pour les options et les noms
    LibererNomsFormates(options);
    for (int i = 0; noms[i]; i++) 
        free(noms[i]);
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