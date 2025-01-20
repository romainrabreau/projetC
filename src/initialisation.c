#include "header.h"
// [numéro du tour] [numéro de la ligne] [type de vilain ennemi]

// présentation du jeu
void VisualiserEnnemis(Etudiant* etudiants, Erreur* erreur) {
    printf("\033[0;0H");
    printf("\033[2J");

    printf("Vagues :\n");

    // Si aucun ennemi
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
    //    Chaque case correspond à [ligne][tour].
    //    On ajoute +1 au maxTour si on veut être certain d'avoir la place,
    //    mais attention à ne pas dépasser NB_ENNEMIS_MAX si vous faites un grand tableau statique.
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
        // Vérifie la validité de la ligne et du tour
        // (si vous utilisez des lignes 1..7 et tours 1..maxTour)
        if (courant->ligne >= 1 && courant->ligne <= NB_LIGNES &&
            courant->tour  >= 1 && courant->tour  <= maxTour) {

            // On place le symbole de l'ennemi (ex: 'Z') dans le tableau
            // Note : on convertit int -> char si besoin
            waves[courant->ligne - 1][courant->tour - 1] = (char)courant->type;
        }
        courant = courant->next;
    }

    // 4) Afficher le tableau
    //    Pour chaque ligne i, on écrit "i| " puis le symbole
    //    (ou '.') pour chaque tour j.
    for (int i = 0; i < NB_LIGNES; i++) {
        // Affiche le numéro de ligne (en partant de 1)
        printf("%d| ", i + 1);

        // Parcourt les tours de 0 à maxTour-1
        for (int j = 0; j < maxTour; j++) {
            printf("%c ", waves[i][j]);
        }
        printf("\n");
    }

    printf("\n");
}

void IntroduireJeu(Erreur *erreur) {
    return;
}