#include "header.h"

#define ANSI_RESET "\033[0m"
#define ANSI_BLUE_BG "\033[44m"
#define ANSI_LIGHT_BLUE_BG "\033[104m"
#define ANSI_WHITE_TEXT "\033[97m"
#define ANSI_BLUE_TEXT "\033[94m"

void VisualiserEnnemis(Etudiant* etudiants, Erreur* erreur) {
    printf("\n");
    printf(ANSI_BLUE_TEXT "Vagues d'ennemis selon les tours et les lignes :" ANSI_RESET "\n\n");

    if (etudiants == NULL) {
        printf(ANSI_LIGHT_BLUE_BG ANSI_WHITE_TEXT "Pas d'etudiants en approche." ANSI_RESET "\n");
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

    printf("  ");
    for (int i = 0; i < maxTour; i++) {
        printf(ANSI_BLUE_TEXT "%2d " ANSI_RESET, i + 1);
    }
    printf("\n");

    for (int i = 0; i <= maxTour; i++) {
        printf(ANSI_BLUE_TEXT "---" ANSI_RESET);
    }
    printf("\n");
    
    for (int i = 0; i < NB_LIGNES; i++) {
        printf(ANSI_BLUE_TEXT "%d| " ANSI_RESET, i + 1);
        for (int j = 0; j < maxTour; j++) {
            if (waves[i][j] == '.') {
                printf(ANSI_LIGHT_BLUE_BG ANSI_WHITE_TEXT "%c  " ANSI_RESET, waves[i][j]);
            } else {
                printf(ANSI_BLUE_BG ANSI_WHITE_TEXT "%c" ANSI_LIGHT_BLUE_BG "  " ANSI_RESET, waves[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void VisualiserTourelles(Tourelle* t) {
    printf("Voici les tourelles disponibles ainsi que leurs caractéristiques :\n\n");
    for (int i = 0; i < NB_TYPES_TOURELLES; i++) {
        printf("%c : %s\n", TYPES_TOURELLES[i].symbole, TYPES_TOURELLES[i].nom);
        printf("    Points de vie : %d\n", TYPES_TOURELLES[i].pointsDeVie);
        printf("    Prix : %d\n\n", TYPES_TOURELLES[i].prix);
    }
}

void IntroduireJeu(Erreur *erreur) {
    printf(ANSI_LIGHT_BLUE_BG ANSI_WHITE_TEXT"        ██╗     ██╗ ██████╗███████╗███╗   ██╗ ██████╗███████╗    ██╗   ██╗███████╗    ███████╗████████╗██╗   ██╗██████╗ ██╗ █████╗ ███╗   ██╗████████╗███████╗      \n");
    printf("        ██║     ██║██╔════╝██╔════╝████╗  ██║██╔════╝██╔════╝    ██║   ██║██╔════╝    ██╔════╝╚══██╔══╝██║   ██║██╔══██╗██║██╔══██╗████╗  ██║╚══██╔══╝██╔════╝      \n");
    printf("        ██║     ██║██║     █████╗  ██╔██╗ ██║██║     █████╗      ██║   ██║███████╗    █████╗     ██║   ██║   ██║██║  ██║██║███████║██╔██╗ ██║   ██║   ███████╗      \n");
    printf("        ██║     ██║██║     ██╔══╝  ██║╚██╗██║██║     ██╔══╝      ╚██╗ ██╔╝╚════██║    ██╔══╝     ██║   ██║   ██║██║  ██║██║██╔══██║██║╚██╗██║   ██║   ╚════██║      \n");
    printf("        ███████╗██║╚██████╗███████╗██║ ╚████║╚██████╗███████╗     ╚████╔╝ ███████║    ███████╗   ██║   ╚██████╔╝██████╔╝██║██║  ██║██║ ╚████║   ██║   ███████║      \n");
    printf("        ╚══════╝╚═╝ ╚═════╝╚══════╝╚═╝  ╚═══╝ ╚═════╝╚══════╝      ╚═══╝  ╚══════╝    ╚══════╝   ╚═╝    ╚═════╝ ╚═════╝ ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝      \n"ANSI_RESET);
    
    animer_attente(2000, CLEAR_LINE);
    print_avec_delai(ANSI_BLUE_TEXT "Bienvenue dans 'Licence vs Étudiant'. \n\n"ANSI_RESET, 50);
    return;
}