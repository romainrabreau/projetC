#include "header.h"
// [numéro du tour] [numéro de la ligne] [type de vilain ennemi]

void VisualiserEnnemis(Etudiant* etudiants, Erreur* erreur) {
    printf("Vagues :\n");

    if (etudiants == NULL) {
        printf("Pas d'etudiants en approche.\n");
        return;
    }

    // 1) Déterminer le nombre de vagues
    int maxTour = 0;
    Etudiant* courant = etudiants;
    while (courant != NULL) {
        if (courant->tour > maxTour) {
            maxTour = courant->tour;
        }
        courant = courant->next;
    }

    // 2) Créer un tableau temporaire pour représenter les ennemis.
    char waves[NB_LIGNES][maxTour];
    // Initialisation à '.'
    for (int i = 0; i < NB_LIGNES; i++) {
        for (int j = 0; j < maxTour; j++) {
            waves[i][j] = '.';
        }
    }

    // 3) Parcourir la liste chainée d'ennemis et remplir le tableau
    courant = etudiants;
    while (courant != NULL) {
        if (courant->ligne >= 1 && courant->ligne <= NB_LIGNES &&
            courant->tour  >= 1 && courant->tour  <= maxTour) {
            waves[courant->ligne - 1][courant->tour - 1] = courant->type;
        }
        courant = courant->next;
    }

    // 4) Afficher le tableau
    for (int i = 0; i < NB_LIGNES; i++) {
        printf("%d| ", i + 1);
        for (int j = 0; j < maxTour; j++) {
            printf("%c  ", waves[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void IntroduireJeu(Erreur *erreur) {
    printf("Bienvenue dans 'Etudiant vs Université'. \n\n");
    return;
}