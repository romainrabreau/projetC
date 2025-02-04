#include "header.h"

// Ajoute au score de la partie courante les points obtenus pour l'élimination d'un ennemi.
void AjouterAuScore(Jeu* jeu, Etudiant* e, Erreur* erreur) {
    if (e == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Etudiant inexistant : pas de score à ajouter.\n");
        return;
    }
    if (e->type == 'Z') {
        jeu->score += 25;
    }
    if (e->type == 'S'|| e->type == 'X') {
        jeu->score += 50;
    }
    if (e->type == 'F') {
        jeu->score += 75;
    }
    if ( e->type == 'L' || e->type == 'D') {
        jeu->score += 100;
    }
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
    mkdir("data_leaderboard", 0755); // Crée le dossier "data_leaderboard" s'il n'existe pas

    char nomBase[MAX_NAME_LEN] = {0};
    int longueur = strlen(jeu->fichier_ennemis);
    int pos = longueur - 1;
    while (pos >= 0 && jeu->fichier_ennemis[pos] != '/')
        pos--;
    if (pos >= 0)
        strcpy(nomBase, jeu->fichier_ennemis + pos + 1); // Conserve uniquement le nom de fichier
    else
        strcpy(nomBase, jeu->fichier_ennemis);

    pos = strlen(nomBase) - 1;
    while (pos >= 0 && nomBase[pos] != '.')
        pos--;
    if (pos >= 0 && strcmp(nomBase + pos, ".txt") == 0)
        nomBase[pos] = '\0';

    char cheminSauvegarde[MAX_NAME_LEN + 100] = {0};
    snprintf(cheminSauvegarde, sizeof(cheminSauvegarde), "data_leaderboard/%s_leaderboard.txt", nomBase);

    int capacite = MAX_SCORES;     
    Jeu scores[MAX_SCORES];
    int nbScores = 0;
    FILE *fichier = fopen(cheminSauvegarde, "r");
    if (fichier != NULL) {
        char ligne[MAX_NAME_LEN] = {0};
        while (fgets(ligne, sizeof(ligne), fichier) && nbScores < capacite) {
            ligne[strcspn(ligne, "\n")] = '\0';
            int i = strlen(ligne) - 1;
            while (i >= 0 && ligne[i] != ' ')
                i--;
            if (i >= 0) {
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
    if (nbScores < capacite) {
        strcpy(scores[nbScores].pseudo, jeu->pseudo); 
        scores[nbScores].score = jeu->score;
        nbScores++;
    } else {
        fprintf(stderr, "Capacité maximale atteinte.\n");
        return;
    }
    qsort(scores, nbScores, sizeof(Jeu), comparerScores);

    fichier = fopen(cheminSauvegarde, "w");  
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
void AfficherLeaderboard(const char *nomLeaderboard, Erreur *erreur) {
    // Construction du chemin du fichier leaderboard : on veut rechercher dans le répertoire data_leaderboard le fichier dont le nom est stocké dans Jeu->fichier_ennemis.
    char cheminLeader[250] = {0};

    if (nomLeaderboard != NULL && nomLeaderboard[0] != '\0') {
        if (strncmp(nomLeaderboard, "data_leaderboard/", 17) != 0)
            snprintf(cheminLeader, sizeof(cheminLeader), "data_leaderboard/%s", nomLeaderboard);
        else
            strcpy(cheminLeader, nomLeaderboard);
    } else {
        printf("Erreur : Aucun leaderboard fourni.\n");
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Aucun leaderboard fourni.");
        return;
    }


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

    // Retire le suffixe "_leaderboard" du nom de fichier pour un affichage plus propre car tous les fichiers de ce dossier on ce suffixe.
    char *pMotif = strstr(nomFichier, "_leaderboard");
    if (pMotif != NULL)
        *pMotif = '\0';
    
    for (int i = 0; i < (int)strlen(nomFichier); i++) {
        if (nomFichier[i] == '_' || nomFichier[i] == '-')
            nomFichier[i] = ' ';
    }

    // Titre de la section Leaderboard (Classement) dans une variable à part pour modifier facilement si besoin. 
    // Attention, à rentrer les dimensions de la figure.

    char *titre[] = {
        " ██████╗██╗      █████╗ ███████╗███████╗███████╗███╗   ███╗███████╗███╗   ██╗████████╗",
        "██╔════╝██║     ██╔══██╗██╔════╝██╔════╝██╔════╝████╗ ████║██╔════╝████╗  ██║╚══██╔══╝",
        "██║     ██║     ███████║███████╗███████╗█████╗  ██╔████╔██║█████╗  ██╔██╗ ██║   ██║   ",
        "██║     ██║     ██╔══██║╚════██║╚════██║██╔══╝  ██║╚██╔╝██║██╔══╝  ██║╚██╗██║   ██║   ",
        "╚██████╗███████╗██║  ██║███████║███████║███████╗██║ ╚═╝ ██║███████╗██║ ╚████║   ██║   ",
        " ╚═════╝╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝   ",
        "                                                                                    "
    };

    int nbLignes = 7;

    printf("\n\n\n\n\n\n\n");
    for (int i = 0; i < nbLignes; i++) {
        printf("\t\t\t\t\t\t\t%s\n", titre[i]); // Centrer le classement
    }

    // Affiche le nom du Niveau
    printf("\n\t\t\t\t\t\t\t"ANSI_TEXTE_BLEU_FONCE"=== [ %s ] ===" ANSI_RESET "\n\n", nomFichier);

    // Lecture et affichage des scores depuis le fichier leaderboard
    FILE *fichier = fopen(cheminLeader, "r");

    if (fichier != NULL) {
        char pseudoBuf[50] = {0};
        int score = 0;
        int rang = 1;
        // On lit MAX_SCORES pseudo au maximum dans le fichier
        while (rang <= MAX_SCORES && fscanf(fichier, "%49s %d", pseudoBuf, &score) == 2) {
            char *medaille = ""; // Pas sur que ça passe au Crio Unix mais on tente non ? :)
            if (rang == 1)
                medaille = "✩ ";
            else if (rang == 2)
                medaille = "✩✩ ";
            else if (rang == 3)
                medaille = "✩✩✩ ";
            // Affiche le rang, la médaille (si applicable), le pseudo et le score, avec mise en forme
            printf("\t\t\t\t\t\t\t%2d. %s%-25s \033[32m%5d pts" ANSI_RESET "\n", rang, medaille, pseudoBuf, score);
            rang++; // Incrémente le rang pour le score suivant
        }
        fclose(fichier);
        // Si aucun score n'a été lu (rang reste 1), on affiche un message indiquant qu'aucun score n'est enregistré
        if (rang == 1)
            printf("\n\t\t\t\t\t\t\t"ANSI_BG_BLEU_MEGA_LIGHT ANSI_TEXTE_BLEU_FONCE"~ Aucun score enregistre ~" ANSI_RESET "\n");
    } else {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Fichier de scores introuvable.");
    } 
    fflush(stdout);
    while(getchar() != '\n');
}