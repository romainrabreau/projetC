#include "header.h"

// Ajoute au score de la partie courante les points obtenus pour l'élimination d'un ennemi.
void AjouterAuScore(Jeu *jeu, Etudiant *e, Erreur *erreur) {
    if (e == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Etudiant inexistant: pas de score.");
        return;
    }
    // Ces valeurs peuvent être modifiées si besoin. Plus l'étudiant est corriace, plus il rapporte.
    if (e->type == 'Z' || e->type == 'S')
        jeu->score += 25;
    if (e->type == 'M' || e->type == 'D' || e->type == 'A')
        jeu->score += 100;
}

// Compare les scores de deux parties pour le tri.
// Si les scores sont égaux, on compare les pseudos par ordre alphabétique.
int comparerScores(const void *a, const void *b) {
    Jeu *jeuA = (Jeu *)a;
    Jeu *jeuB = (Jeu *)b;
    int difference = jeuB->score - jeuA->score;
    if (difference == 0)
        return strcmp(jeuA->pseudo, jeuB->pseudo);
    return difference;
}

// Ajoute le score courant au leaderboard à l'issue d'une partie gagnée.
// 1) On crée le dossier "data_leaderboard" s'il n'existe pas. C'est là que l'on va mettre les classements.
// 2) Puis on extrait le 'nom de base' du niveau à partir du chemin de fichier, sans les extensions et Répertoires 
// car on va ouvrir (ou créer si il n'existe pas) un fichier .txt pour stocker les 10 meilleurs classements. 
// (Limite modifiable dans le header)
// 3) On lit les scores existants, ajoute le nouveau score, trie et réécrit le fichier.
// avec le format [Pseudo] [Score] 
void AjouterAuLeaderboard(Jeu *jeu, Erreur* erreur) {       
    // Etape 1  
    // Création du dossier "data_leaderboard" s'il n'existe pas
    mkdir("data_leaderboard", 0755);

    // Etape 2
    // On suppose que le nom fait moins de 250 caractères. 
    char nomBase[250] = {0};
    int longueur = strlen(jeu->fichier_ennemis);

    // Recherche du dernier '/' dans le chemin en partant de la droite pour le tronquer.
    int pos = longueur - 1;
    while (pos >= 0 && jeu->fichier_ennemis[pos] != '/') {
        pos--;
    }  
    if (pos >= 0)
        strcpy(nomBase, jeu->fichier_ennemis + pos + 1); // On garde que le nom du fichier sans répertoire avant.
    else
        strcpy(nomBase, jeu->fichier_ennemis); // aucun '/' trouvé, donc on a déjà le nom voulu.

    // On retire l'extension ".txt" si elle est présente car on va ajouter le suffixe _leaderboard.txt
    pos = strlen(nomBase) - 1;  
    while (pos >= 0 && nomBase[pos] != '.') {
        pos--;
    }
    if (pos >= 0 && strcmp(nomBase + pos, ".txt") == 0)
        nomBase[pos] = '\0';

    // On assemble la nouvelle adresse vers le Leaderboard
    char cheminLeader[256] = {0};
    snprintf(cheminLeader, sizeof(cheminLeader), "data_leaderboard/%s_leaderboard.txt", nomBase);

    // On lit MAX_SCORES + un nombre arbitraire, si des classmements ont étés rentrés à la main par exemple. (Pour les prendre en compte dans le tri)
    int capacite = MAX_SCORES + 5;     
    Jeu scores[MAX_SCORES + 5];
    int nbScores = 0;

    // Ouvre le fichier avec l'adresse créer avant en lecture
    FILE *fichier = fopen(cheminLeader, "r");
    if (fichier != NULL) {
        char ligne[MAX_SCORES + 5];  // Buffer pour stocker chaque ligne du fichier
        while (fgets(ligne, sizeof(ligne), fichier) && nbScores < capacite) {
            // Retire le saut de ligne si il s'est glissé dans le fichier.
            ligne[strcspn(ligne, "\n")] = '\0';

            // Recherche le dernier caractère espace dans la ligne, pour séparer le pseudo du score
            int i = strlen(ligne) - 1;
            while (i >= 0 && ligne[i] != ' ')
                i--;

            if (i >= 0) {
                // On termine la chaîne à la position de l'espace pour isoler le pseudo
                ligne[i] = '\0';

                // Conversion de la chaîne située après l'espace en entier
                int sc = 0;
                sscanf(ligne + i + 1, "%d", &sc);

                // Copie du pseudo dans le tableau et enregistrement du score
                strcpy(scores[nbScores].pseudo, ligne);
                scores[nbScores].score = sc;
                nbScores++;
            }   
        }
        fclose(fichier);
    }
    // Ajoute le score courant de fin de partie à la fin du tableau
    if (nbScores <= capacite) {
        strcpy(scores[nbScores].pseudo, jeu->pseudo); 
        scores[nbScores].score = jeu->score;
        nbScores++;
    } else {
        fprintf(stderr, "Capacité maximale atteinte.\n");
        return;
    }
    // Trie le tableau des scores dans l'odre croissant.
    qsort(scores, nbScores, sizeof(Jeu), comparerScores);

    // Réécrit le fichier leaderboard avec les 'MAX_SCORES' meilleurs scores
    fichier = fopen(cheminLeader, "w");  
    if (fichier != NULL) {
        for (int i = 0; i < nbScores && i < MAX_SCORES; i++) {
            fprintf(fichier, "%s %d\n", scores[i].pseudo, scores[i].score);
        } 
        fclose(fichier);
    } else { 
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Erreur lors de l'écriture du leaderboard");
        return;
    }
}

// Affiche le leaderboard demandé avec 'fichier_ennemis' dans terminal.
void AfficherLeaderboard(Jeu *jeu) {   
    // Construction du chemin du fichier leaderboard : on veut rechercher dans le répertoire data_leaderboard le fichier dont le nom est stocké dans Jeu->fichier_ennemis.
    char cheminLeader[250] = {0};
    if (jeu != NULL && jeu->fichier_ennemis[0] != '\0') {
        // Vérifie si le chemin contenu dans jeu->fichier_ennemis commence par "data_leaderboard/" car si oui pas la peine d'ajouter le prexixe. 
        if (strncmp(jeu->fichier_ennemis, "data_leaderboard/", 17) != 0)
            snprintf(cheminLeader, sizeof(cheminLeader), "data_leaderboard/%s", jeu->fichier_ennemis);
        else
            strcpy(cheminLeader, jeu->fichier_ennemis);
    } else {
        // Si jeu est vide, ou si le champ fichier_ennemis est vide : 
        printf("Erreur : Aucun leaderboard fourni.\n"); 
        return;
    }
    // TODO : Possibilité de fusionner cette action dans une fonction pour réduire le code (car utilisé aussi plus haut)
    // On extrait le nom de fichier à afficher en supprimant le chemin de répertoire
    char nomFichier[250] = {0};
    int posSlash = -1; // Variable pour stocker la position du dernier '/'
    int lenChemin = strlen(cheminLeader);
    // On parcourt la chaîne depuis la fin pour trouver le dernier '/' 
    for (int i = lenChemin - 1; i >= 0; i--) {
        if (cheminLeader[i] == '/') {
            posSlash = i;
            break;
        }
    }
    // Si un '/' a été trouvé, on copie la partie après ce caractère dans nomFichier, pour tronquer
    // sinon on copie l'intégralité de cheminLeader (car il ne contient pas de répertoire)
    if (posSlash >= 0)
        strcpy(nomFichier, cheminLeader + posSlash + 1);
    else
        strcpy(nomFichier, cheminLeader);

    // Retire le suffixe "_leaderboard" du nom de fichier pour un affichage plus propre car tous les fichiers de ce dossier on ce suffixe.
    char *pMotif = strstr(nomFichier, "_leaderboard");
    if (pMotif != NULL)
        *pMotif = '\0';

    // Remplace les '_' et '-' par des espaces dans le nom pour une meilleure lisibilité
    for (int i = 0; i < (int)strlen(nomFichier); i++) {
        if (nomFichier[i] == '_' || nomFichier[i] == '-')
            nomFichier[i] = ' ';
    }

    // Titre de la section Leaderboard (Classement) dans une variable à part pour modifier facilement si besoin. 
    // Attention, à rentrer les dimensions de la figure.
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
        printf("\t\t\t\t\t\t\t%s\n", titre[i]); // Centrer le classement
    }

    // Affiche le nom du Niveau
    printf("\n\t\t\t\t\t\t\t\033[1;36m=== [ %s ] ===\033[0m\n\n", nomFichier);

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
                medaille = "🏆 ";
            else if (rang == 2)
                medaille = "🥈 ";
            else if (rang == 3)
                medaille = "🥉 ";
            // Affiche le rang, la médaille (si applicable), le pseudo et le score, avec mise en forme
            printf("\t\t\t\t\t\t\t\033[37m%2d. %s%-25s \033[32m%5d pts\033[0m\n", rang, medaille, pseudoBuf, score);
            rang++; // Incrémente le rang pour le score suivant
        }
        fclose(fichier);
        // Si aucun score n'a été lu (rang reste 1), on affiche un message indiquant qu'aucun score n'est enregistré
        if (rang == 1)
            printf("\n\t\t\t\t\t\t\t\033[33m~ Aucun score enregistre ~\033[0m\n");
    } else {
        printf("\n\t\t\t\t\t\t\t\033[31m⚠ Fichier de scores introuvable\033[0m\n");
    }
    printf("\n\n\t\t\t\t\t\t\t\033[2;3m[Appuyez sur Entrree pour continuer...]\033[0m");
    fflush(stdout);
    while(getchar() != '\n');
}

