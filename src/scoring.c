#include "header.h"

// Ajoute au score de la partie courante les points obtenus pour l'élimination d'un ennemi.
void AjouterAuScore(Jeu *jeu, Etudiant *e, Erreur *erreur) {
    if (e == NULL) {
        if (erreur) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Etudiant inexistant: pas de score.");
        }
        return;
    }
    // Ces valeurs peuvent être modifiées si besoin. Plus l'étudiant est coriace, plus il rapporte.
    if (e->type == 'Z' || e->type == 'S')
        jeu->score += 25;
    if (e->type == 'M' || e->type == 'D' || e->type == 'A')
        jeu->score += 100;
}

// Compare les scores de deux parties pour le tri.
// Si les scores sont égaux, on compare les pseudos par ordre alphabétique.
int comparerScores(const void *a, const void *b) {
    const Jeu *jeuA = (const Jeu *)a;
    const Jeu *jeuB = (const Jeu *)b;
    int difference = jeuB->score - jeuA->score;
    if (difference == 0)
        return strcmp(jeuA->pseudo, jeuB->pseudo);
    return difference;
}

// Ajoute le score courant au leaderboard à l'issue d'une partie gagnée.
void AjouterAuLeaderboard(Jeu *jeu, Erreur *erreur) {       
    /* Étape 1 : Création du dossier "data_leaderboard" s'il n'existe pas */
    mkdir("data_leaderboard", 0755);

    /* Étape 2 : Extraction du nom de base du niveau à partir du chemin stocké dans jeu->fichier_ennemis */
    char nomBase[250] = {0};
    int longueur = strlen(jeu->fichier_ennemis);
    int pos = longueur - 1;
    while (pos >= 0 && jeu->fichier_ennemis[pos] != '/')
        pos--;
    if (pos >= 0)
        strcpy(nomBase, jeu->fichier_ennemis + pos + 1); // Conserver uniquement le nom de fichier
    else
        strcpy(nomBase, jeu->fichier_ennemis);

    /* Retirer l'extension ".txt" si elle est présente */
    pos = strlen(nomBase) - 1;
    while (pos >= 0 && nomBase[pos] != '.')
        pos--;
    if (pos >= 0 && strcmp(nomBase + pos, ".txt") == 0)
        nomBase[pos] = '\0';

    /* Assemblage du chemin complet du fichier leaderboard */
    char cheminLeader[256] = {0};
    snprintf(cheminLeader, sizeof(cheminLeader), "data_leaderboard/%s_leaderboard.txt", nomBase);

    /* Étape 3 : Lecture des scores existants */
    int capacite = MAX_SCORES + 5;     
    Jeu scores[MAX_SCORES + 5];
    int nbScores = 0;
    FILE *fichier = fopen(cheminLeader, "r");
    if (fichier != NULL) {
        char ligne[256] = {0};  // Buffer pour stocker chaque ligne du fichier
        while (fgets(ligne, sizeof(ligne), fichier) && nbScores < capacite) {
            ligne[strcspn(ligne, "\n")] = '\0';  // Supprimer le saut de ligne
            int i = strlen(ligne) - 1;
            while (i >= 0 && ligne[i] != ' ')
                i--;
            if (i >= 0) {
                /* Isolation du pseudo et conversion du score */
                char pseudoLu[50] = {0};
                strncpy(pseudoLu, ligne, i);
                pseudoLu[i] = '\0';
                int sc = 0;
                sscanf(ligne + i + 1, "%d", &sc);
                strcpy(scores[nbScores].pseudo, pseudoLu);
                scores[nbScores].score = sc;
                nbScores++;
            }   
        }
        fclose(fichier);
    }
    /* Ajout du score courant */
    if (nbScores < capacite) {
        strcpy(scores[nbScores].pseudo, jeu->pseudo); 
        scores[nbScores].score = jeu->score;
        nbScores++;
    } else {
        fprintf(stderr, "Capacité maximale atteinte.\n");
        return;
    }
    /* Trie des scores par ordre décroissant */
    qsort(scores, nbScores, sizeof(Jeu), comparerScores);

    /* Étape 4 : Réécriture du fichier leaderboard avec les MAX_SCORES meilleurs scores */
    fichier = fopen(cheminLeader, "w");  
    if (fichier != NULL) {
        for (int i = 0; i < nbScores && i < MAX_SCORES; i++) {
            fprintf(fichier, "%s %d\n", scores[i].pseudo, scores[i].score);
        } 
        fclose(fichier);
    } else { 
        if (erreur) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Erreur lors de l'écriture du leaderboard");
        }
        return;
    }
}

// Affiche le leaderboard demandé dans le terminal.
void AfficherLeaderboard(const char *nomLeaderboard, Erreur *err) {
    char cheminLeader[MAX_NAME_LEN] = {0};
    
    /* Vérification du paramètre nomLeaderboard */
    if (nomLeaderboard != NULL && nomLeaderboard[0] != '\0') {
        if (strncmp(nomLeaderboard, "data_leaderboard/", 17) != 0)
            snprintf(cheminLeader, sizeof(cheminLeader), "data_leaderboard/%s", nomLeaderboard);
        else
            strcpy(cheminLeader, nomLeaderboard);
    } else {
        printf("Erreur : Aucun leaderboard fourni.\n");
        if (err) {
            snprintf(err->msg_erreur, sizeof(err->msg_erreur), "Erreur : Aucun leaderboard fourni.");
            err->statut_erreur = 1;
        }
        return;
    }
    
    /* Extraction du nom de fichier sans le chemin de répertoire */
    char nomFichier[MAX_NAME_LEN] = {0};
    int posSlash = -1;
    int lenChemin = strlen(cheminLeader);
    for (int i = lenChemin - 1; i >= 0; i--) {
        if (cheminLeader[i] == '/') {
            posSlash = i;
            break;
        }
    }
    if (posSlash >= 0)
        strcpy(nomFichier, cheminLeader + posSlash + 1);
    else
        strcpy(nomFichier, cheminLeader);
    
    /* Retirer le suffixe "_leaderboard" pour un affichage plus propre */
    char *pMotif = strstr(nomFichier, "_leaderboard");
    if (pMotif != NULL)
        *pMotif = '\0';
    
    /* Remplacer les caractères '_' et '-' par des espaces */
    for (int i = 0; i < (int)strlen(nomFichier); i++) {
        if (nomFichier[i] == '_' || nomFichier[i] == '-')
            nomFichier[i] = ' ';
    }
    
    /* Affichage du titre stylisé */
    char *titre[] = {
        "  ____ _                                         _   ",
        " / ___| | __ _ ___ ___  ___ _ __ ___   ___ _ __ | |_ ",
        "| |   | |/ _` / __/ __|/ _ \\ '_ ` _ \\ / _ \\ '_ \\| __|",
        "| |___| | (_| \\__ \\__ \\  __/ | | | | |  __/ | | | |_ ",
        " \\____|_|\\__,_|___/___/\\___|_| |_| |_|\\___|_| |_|\\__|"
    };
    int nbLignes = 5;
    
    printf("\n\n\n\n\n\n\n");
    for (int i = 0; i < nbLignes; i++) {
        printf("\t\t\t\t\t\t\t%s\n", titre[i]);
    }
    
    printf("\n\t\t\t\t\t\t\t\033[1;36m=== [ %s ] ===\033[0m\n\n", nomFichier);
    
    /* Lecture et affichage des scores depuis le fichier leaderboard */
    FILE *fichier = fopen(cheminLeader, "r");
    if (fichier != NULL) {
        char pseudoBuf[50] = {0};
        int score = 0;
        int rang = 1;
        while (rang <= MAX_SCORES && fscanf(fichier, "%49s %d", pseudoBuf, &score) == 2) {
            const char *medaille = "";
            if (rang == 1)
                medaille = "🏆 ";
            else if (rang == 2)
                medaille = "🥈 ";
            else if (rang == 3)
                medaille = "🥉 ";
            
            printf("\t\t\t\t\t\t\t\033[37m%2d. %s%-25s \033[32m%5d pts\033[0m\n", rang, medaille, pseudoBuf, score);
            rang++;
        }
        fclose(fichier);
        if (rang == 1)
            printf("\n\t\t\t\t\t\t\t\033[33m~ Aucun score enregistre ~\033[0m\n");
    } else {
        printf("\n\t\t\t\t\t\t\t\033[31m⚠ Fichier de scores introuvable\033[0m\n");
        if (err) {
            snprintf(err->msg_erreur, sizeof(err->msg_erreur), "Fichier de scores introuvable : %s", cheminLeader);
            err->statut_erreur = 1;
        }
    }
    
    printf("\n\n\t\t\t\t\t\t\t\033[2;3m[Appuyez sur Entrree pour continuer...]\033[0m");
    fflush(stdout);
    while(getchar() != '\n');
}
