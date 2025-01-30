#include "header.h"


void VisualiserEnnemis(Etudiant* etudiants, Erreur* erreur) {
    printf("\n");
    printf(ANSI_TEXTE_BLEU_MOYEN "Vagues d'ennemis selon les tours et les lignes :" ANSI_RESET "\n\n");

    if (etudiants == NULL) {
        printf(ANSI_BG_BLEU_CLAIR ANSI_TEXTE_BLANC "Pas d'etudiants en approche." ANSI_RESET "\n");
        return;
    }

    int maxTour = 0;
    Etudiant* courant = etudiants;
    while (courant != NULL) {
        if (courant->tour > maxTour) {
            maxTour = courant->tour;
        }
        courant = courant->next;
    }

    char waves[NB_LIGNES][maxTour];
    for (int i = 0; i < NB_LIGNES; i++) {
        for (int j = 0; j < maxTour; j++) {
            waves[i][j] = '.';
        }
    }

    courant = etudiants;
    while (courant != NULL) {
        if (courant->ligne >= 1 && courant->ligne <= NB_LIGNES &&
            courant->tour >= 1 && courant->tour <= maxTour) {
            waves[courant->ligne - 1][courant->tour - 1] = courant->type;
        }
        courant = courant->next;
    }

    for (int i = 0; i < NB_LIGNES; i++) {
        printf(ANSI_TEXTE_BLEU_MOYEN "%d| " ANSI_RESET, i + 1);
        for (int j = 0; j < maxTour; j++) {
            if (waves[i][j] == '.') {
                printf(ANSI_BG_BLEU_CLAIR ANSI_TEXTE_BLANC "%c  " ANSI_RESET, waves[i][j]);
            } else {
                printf(ANSI_BG_VERT_FONCE ANSI_TEXTE_BLANC "%c" ANSI_BG_BLEU_CLAIR "  " ANSI_RESET, waves[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void IntroduireJeu(Erreur *erreur) {
    printf(ANSI_BG_BLEU_SHINY ANSI_TEXTE_BLANC"██╗     ██╗ ██████╗███████╗███╗   ██╗ ██████╗███████╗    ██╗   ██╗███████╗    ███████╗████████╗██╗   ██╗██████╗ ██╗ █████╗ ███╗   ██╗████████╗███████╗\n");
    printf("██║     ██║██╔════╝██╔════╝████╗  ██║██╔════╝██╔════╝    ██║   ██║██╔════╝    ██╔════╝╚══██╔══╝██║   ██║██╔══██╗██║██╔══██╗████╗  ██║╚══██╔══╝██╔════╝\n");
    printf("██║     ██║██║     █████╗  ██╔██╗ ██║██║     █████╗      ██║   ██║███████╗    █████╗     ██║   ██║   ██║██║  ██║██║███████║██╔██╗ ██║   ██║   ███████╗\n");
    printf("██║     ██║██║     ██╔══╝  ██║╚██╗██║██║     ██╔══╝      ╚██╗ ██╔╝╚════██║    ██╔══╝     ██║   ██║   ██║██║  ██║██║██╔══██║██║╚██╗██║   ██║   ╚════██║\n");
    printf("███████╗██║╚██████╗███████╗██║ ╚████║╚██████╗███████╗     ╚████╔╝ ███████║    ███████╗   ██║   ╚██████╔╝██████╔╝██║██║  ██║██║ ╚████║   ██║   ███████║\n");
    printf("╚══════╝╚═╝ ╚═════╝╚══════╝╚═╝  ╚═══╝ ╚═════╝╚══════╝      ╚═══╝  ╚══════╝    ╚══════╝   ╚═╝    ╚═════╝ ╚═════╝ ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝\n"ANSI_RESET);
    printf("\n");
    animer_attente(1000, CLEAR_LINE);
    print_avec_delai(ANSI_TEXTE_BLEU_FONCE "Bienvenue dans 'Licence vs Étudiants'. \n\n\n"ANSI_RESET, 50);
    printf("Vous êtes le responsable de la licence informatique de l'Université PSL - Pixel Science-Logique .\n");
    printf("blablabla\n\n");
    return;
}