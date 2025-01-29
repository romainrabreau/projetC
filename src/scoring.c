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
    return (jb->score - ja->score);
}

void AddToLeaderboard(Jeu *jeu) {
    FILE *f;
    Jeu scores[MAX_SCORES + 1];
    int size = 0;

    char path[256];
    snprintf(path, sizeof(path), "data_leaderboard/%s_leaderboard.txt", jeu->fichier_ennemis);

    // Lecture de l’existant
    f = fopen(path, "r");
    if (f != NULL) {
        while (size < MAX_SCORES &&
               fscanf(f, "%49s %d", scores[size].pseudo, &scores[size].score) == 2) {
            size++;
        }
        fclose(f);
    }

    strcpy(scores[size].pseudo, jeu->pseudo);
    scores[size].score = jeu->score;
    size++;

    // Tri décroissant
    qsort(scores, size, sizeof(Jeu), compareScores);

    // Tronque à 10 si nécessaire
    if (size > MAX_SCORES) {
        size = MAX_SCORES;
    }

    // Écriture
    f = fopen(path, "w");
    if (f != NULL) {
        for (int i = 0; i < size; i++) {
            fprintf(f, "%s %d\n", scores[i].pseudo, scores[i].score);
        }
        fclose(f);
    }
}

void AfficherLeaderboard() {
    FILE *f;
    char pseudo[50];
    int score;
    int i = 0;

    f = fopen("leaderboard.txt", "r");
    if (f == NULL) {
        printf("Impossible d'ouvrir 'leaderboard.txt'\n");
        return;
    }

    printf("===== LEADERBOARD =====\n");
    // Lecture de chaque ligne du fichier
    while (i < MAX_SCORES && fscanf(f, "%49s %d", pseudo, &score) == 2) {
        printf("%2d) %s - %d\n", i + 1, pseudo, score);
        i++;
    }
    fclose(f);
}