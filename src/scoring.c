#include "header.h"

void AddToScore(Jeu* jeu, Etudiant* e, Erreur* erreur) {
    if (e == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Etudiant inexistant : pas de score à ajouter.\n");
        return;
    }
    if (e->type == 'Z' ||
        e->type == 'S') {
        jeu->score += 25;
    }
    if (e->type == 'M' ||
        e->type == 'D') {
        jeu->score += 100;
    }
    if (e->type == 'A') {
        jeu->score += 100;
    }
}

int compareScores(const void *a, const void *b) {
    const Jeu *ja = (const Jeu *)a;
    const Jeu *jb = (const Jeu *)b;
    int diff = jb->score - ja->score;
    if(diff == 0) {
        // On utilise strcmp pour départager (ordre alphabétique croissant)
        return strcmp(ja->pseudo, jb->pseudo);
    }
    return diff;
}

void AddToLeaderboard(Jeu *jeu) {
    // Créer le dossier s'il n'existe pas
    mkdir("data_leaderboard", 0755);

    // Récupérer le nom de fichier de jeu (en retirant le chemin s'il y en a un)
    const char* nom_niveau = strrchr(jeu->fichier_ennemis, '/');
    nom_niveau = nom_niveau ? nom_niveau + 1 : jeu->fichier_ennemis;
    
    // Retirer l'extension ".txt" si elle existe
    char nom_base[256] = {0};
    strncpy(nom_base, nom_niveau, sizeof(nom_base) - 1);
    char *dot = strrchr(nom_base, '.');
    if(dot && strcmp(dot, ".txt") == 0) {
        *dot = '\0';
    }

    // Construit le chemin complet du fichier leaderboard.
    // Attention : si dans data_leaderboard les fichiers sont nommés, par exemple,
    // "1_Des_Rebelles_A_Dauphine_leaderboard" (sans extension),
    // adaptez cette ligne en conséquence.
    char path[256];
    snprintf(path, sizeof(path), "data_leaderboard/%s_leaderboard.txt", nom_base);

    // Préparation d'un tableau temporaire pour stocker les scores existants + le nouveau
    const int capacity = 1024;
    Jeu scores[capacity];
    int size = 0;

    // Lecture du fichier existant ligne par ligne
    FILE *f = fopen(path, "r");
    if (f) {
        char line[256];
        while (fgets(line, sizeof(line), f) && size < capacity) {
            // Enlever le saut de ligne s'il existe
            line[strcspn(line, "\n")] = '\0';
            // Pour extraire le score, on considère que c'est le dernier élément de la ligne.
            // On cherche le dernier espace.
            char *last_space = strrchr(line, ' ');
            if (last_space) {
                *last_space = '\0'; // Fin de la chaîne pour le pseudo
                int sc = atoi(last_space + 1);
                // On copie le pseudo (la partie restante de la ligne)
                strncpy(scores[size].pseudo, line, sizeof(scores[size].pseudo) - 1);
                scores[size].pseudo[sizeof(scores[size].pseudo) - 1] = '\0';
                scores[size].score = sc;
                size++;
            }
        }
        fclose(f);
    }

    // Ajout du nouveau score
    if (size < capacity) {
        strncpy(scores[size].pseudo, jeu->pseudo, sizeof(scores[size].pseudo) - 1);
        scores[size].pseudo[sizeof(scores[size].pseudo) - 1] = '\0';
        scores[size].score = jeu->score;
        size++;
    } else {
        fprintf(stderr, "Capacité maximale de scores atteinte.\n");
        return;
    }

    // Tri des scores (du meilleur au moins bon)
    qsort(scores, size, sizeof(Jeu), compareScores);

    // Réécriture du fichier avec les MAX_SCORES meilleurs scores
    f = fopen(path, "w");
    if (f) {
        // On verrouille le fichier pendant l'écriture
        flock(fileno(f), LOCK_EX);
        for (int i = 0; i < size && i < MAX_SCORES; i++) {
            fprintf(f, "%s %d\n", scores[i].pseudo, scores[i].score);
        }
        flock(fileno(f), LOCK_UN);
        fclose(f);
    } else {
        perror("Erreur lors de l'écriture du leaderboard");
    }
}

void AfficherLeaderboard(Jeu* jeu) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int terminal_width = (w.ws_col > 0) ? w.ws_col : 80;
    int padding = (terminal_width - 50) / 2;
    if(padding < 0) padding = 0;

    char path[256] = {0};
    char nom_formate[100] = {0};

    if(jeu && jeu->fichier_ennemis[0]) {
        // Si le chemin ne commence pas déjà par "data_leaderboard/", on le préfixe.
        if (strncmp(jeu->fichier_ennemis, "data_leaderboard/", 17) != 0)
            snprintf(path, sizeof(path), "data_leaderboard/%s", jeu->fichier_ennemis);
        else
            snprintf(path, sizeof(path), "%s", jeu->fichier_ennemis);

        // Pour l'affichage du titre, on extrait le nom de fichier sans le dossier
        const char* slash = strrchr(path, '/');
        const char* nom_fichier = slash ? slash + 1 : path;
        
        // Copie dans une variable temporaire pour retraiter le nom
        char baseName[100] = {0};
        strncpy(baseName, nom_fichier, sizeof(baseName) - 1);
        
        // Suppression du suffixe "_leaderboard" s'il est présent
        char* pos = strstr(baseName, "_leaderboard");
        if(pos) {
            *pos = '\0';
        }
        
        // Formatage du nom pour affichage : remplacement des underscores et tirets par des espaces
        strncpy(nom_formate, baseName, sizeof(nom_formate) - 1);
        for(char* p = nom_formate; *p; p++) {
            if(*p == '_' || *p == '-')
                *p = ' ';
        }
    }
    else {
        printf("Erreur : Aucun leaderboard fourni.\n");
        return;
    }

    // Affichage de l'ASCII Art "CLASSEMENT"
    const char* CLASSEMENT[] = {
        "  ____ _                                         _   ",
        " / ___| | __ _ ___ ___  ___ _ __ ___   ___ _ __ | |_ ",
        "| |   | |/ _` / __/ __|/ _ \\ '_ ` _ \\ / _ \\ '_ \\| __|",
        "| |___| | (_| \\__ \\__ \\  __/ | | | | |  __/ | | | |_ ",
        " \\____|_|\\__,_|___/___/\\___|_| |_| |_|\\___|_| |_|\\__|"
    };
    const int nb_lignes_classement = sizeof(CLASSEMENT) / sizeof(CLASSEMENT[0]);

    // Efface l'écran et repositionne le curseur
    printf("\033[2J\033[0;0H");
    for(int i = 0; i < nb_lignes_classement; i++) {
        printf("%*s%s\n", padding, "", CLASSEMENT[i]);
    }
    
    // Si aucun nom n'a été extrait, utiliser un titre par défaut
    if(strlen(nom_formate) == 0) {
        strncpy(nom_formate, "CLASSEMENT GÉNÉRAL", sizeof(nom_formate) - 1);
    }
    printf("\n%*s\033[1;36m=== [ %s ] ===\033[0m\n\n", padding-8, "", nom_formate);

    // Lecture et affichage des scores depuis le fichier
    FILE* f = fopen(path, "r");
    if(f) {
        char pseudo[50] = {0};
        int score = 0, rank = 1;
        
        while(rank <= MAX_SCORES && fscanf(f, "%49s %d", pseudo, &score) == 2) {
            const char* medal = "";
            if(rank == 1) medal = "🏆 ";
            else if(rank == 2) medal = "🥈 ";
            else if(rank == 3) medal = "🥉 ";
            
            printf("%*s\033[37m%2d. %s%-25s \033[32m%5d pts\033[0m\n", 
                   padding, "", rank, medal, pseudo, score);
            rank++;
        }
        fclose(f);
        
        if(rank == 1) {
            printf("\n%*s\033[33m~ Aucun score enregistré ~\033[0m\n", padding+5, "");
        }
    } else {
        printf("%*s\033[31m⚠ Fichier de scores introuvable\033[0m\n", padding+5, "");
    }

    printf("\n\n%*s\033[2;3m[Appuyez sur Entrée pour continuer...]\033[0m", padding+10, "");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Vidange du buffer
}