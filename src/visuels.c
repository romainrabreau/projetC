#include "header.h"

void AnimerAttente(int attente_ms, char *message) {
    char spin_chars[] = {'|', '/', '-', '\\'};
    int steps = attente_ms / 200;  // ajustement de la vitesse de rotation

    printf(" %s", message);
    fflush(stdout);

    for (int i = 0; i < steps; i++) {
        printf("\r%c", spin_chars[i % 4]);
        fflush(stdout); // force l'affichage
        usleep(200000);  // 200ms attente
    }
    printf("\r");  // efface le dernier caractère
    fflush(stdout);
    printf("%s\n", message);
}

void printAvecDelai(const char *text, int delai_ms) {
    while (*text) {
        printf("%c", *text++);
        fflush(stdout);
        usleep(delai_ms * 1000);
    }
}

# define LONG_BARRE 50
void BarreChargement(int ms) {
    // affiche une barre qui se remplit progressivement
    int steps = LONG_BARRE;
    int step_time = ms / steps;  // time par tour de barre

    printf("|");  // début de la barre
    for (int i = 0; i < LONG_BARRE; i++) {
        printf(" "); // au début, la barre est vide
    }
    printf("|"); 

    fflush(stdout); // envoie la sortie sur le terminal

    for (int i = 0; i <= LONG_BARRE; i++) {
        printf("\r|");  // retourne au début de la ligne
        for (int j = 0; j < i; j++) {
            printf(ANSI_BG_BLEU_CLAIR " " ANSI_RESET);  // bloc de couleur de chargement
        }
        for (int j = i; j < LONG_BARRE; j++) {
            printf(" ");
        }
        printf("|");
        fflush(stdout);
        usleep(step_time * 1000);
    }
    printf("\n"); 
}

void IntroMenu() {
    AnimerAttente(100, CLEAR_LINE);
    printAvecDelai(ANSI_TEXTE_BLEU_FONCE "Bienvenue dans 'Licence vs Étudiants'.\n" ANSI_RESET, 50);
    AnimerAttente(100, CLEAR_LINE);
    printAvecDelai(ANSI_TEXTE_BLEU_FONCE "Chargement du Menu... \n" ANSI_RESET, 50);
    AnimerAttente(200, CLEAR_LINE);
}

void AfficherTitre() {
    printf(ANSI_BG_BLEU_SHINY ANSI_TEXTE_BLANC"                ██╗     ██╗ ██████╗███████╗███╗   ██╗ ██████╗███████╗    ██╗   ██╗███████╗    ███████╗████████╗██╗   ██╗██████╗ ██╗ █████╗ ███╗   ██╗████████╗███████╗                \n");
    printf("                ██║     ██║██╔════╝██╔════╝████╗  ██║██╔════╝██╔════╝    ██║   ██║██╔════╝    ██╔════╝╚══██╔══╝██║   ██║██╔══██╗██║██╔══██╗████╗  ██║╚══██╔══╝██╔════╝                \n");
    printf("                ██║     ██║██║     █████╗  ██╔██╗ ██║██║     █████╗      ██║   ██║███████╗    █████╗     ██║   ██║   ██║██║  ██║██║███████║██╔██╗ ██║   ██║   ███████╗                \n");
    printf("                ██║     ██║██║     ██╔══╝  ██║╚██╗██║██║     ██╔══╝      ╚██╗ ██╔╝╚════██║    ██╔══╝     ██║   ██║   ██║██║  ██║██║██╔══██║██║╚██╗██║   ██║   ╚════██║                \n");
    printf("                ███████╗██║╚██████╗███████╗██║ ╚████║╚██████╗███████╗     ╚████╔╝ ███████║    ███████╗   ██║   ╚██████╔╝██████╔╝██║██║  ██║██║ ╚████║   ██║   ███████║                \n");
    printf("                ╚══════╝╚═╝ ╚═════╝╚══════╝╚═╝  ╚═══╝ ╚═════╝╚══════╝      ╚═══╝  ╚══════╝    ╚══════╝   ╚═╝    ╚═════╝ ╚═════╝ ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝                \n"ANSI_RESET);
    printf("\n");
}

int AfficherChoix(char **options, int n_options) {
    // à partir d'un tableau d'options affiche un menu et renvoie le choix de l'utilisateur
    int choix = -1;

    while (1) {
        int max_length = 0;
        for (int i = 0; i < n_options; ++i) {
            int len = strlen(options[i]) + 3;
            if (len > max_length) {
                max_length = len;
            }
        }

        // barrière du haut
        printf("\t\t\t\t\t\t\t---");
        for (int i = 0; i < max_length; i++) {
            printf("-");
        }
        printf("---\n");

        // Liste des options
        for (int i = 0; i < n_options; ++i) {
            printf("\t\t\t\t\t\t\t");
            printf("   %d. %s\n", i + 1, options[i]);
        }

        // barrière du bas
        printf("\t\t\t\t\t\t\t---");
        for (int i = 0; i < max_length; i++) {
            printf("-");
        }
        printf("---\n\n");

        // Lecture du choix
        printf("\t\t\t\t\t\t\tVeuillez sélectionner une option (1-%d) : ", n_options);

        int choisi;
        while (1) {
            scanf("%d", &choisi);
            if (choisi >= 1 && choisi <= n_options) {
                printf("\n");
                return choisi - 1;
            } else {
                printf("\t\t\t\t\t\t\tOption invalide. Veuillez entrer un nombre entre 1 et %d.\n", n_options);
            }
        }
    }
}


void VisualiserEnnemis(Etudiant* etudiants, Erreur* erreur) {
    printf("\n");
    printf(ANSI_TEXTE_BLEU_MOYEN "Vagues d'ennemis selon les tours et les lignes :" ANSI_RESET "\n\n");

    if (etudiants == NULL) {
        printf(ANSI_BG_BLEU_FONCE ANSI_TEXTE_BLANC "Pas d'étudiants en approche." ANSI_RESET "\n");
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
        printf(ANSI_TEXTE_BLEU_FONCE "%2d " ANSI_RESET, i + 1);
    }
    printf("\n");

    for (int i = 0; i <= maxTour; i++) {
        printf(ANSI_TEXTE_BLEU_FONCE "---" ANSI_RESET);
    }
    printf("\n");
    
    for (int i = 0; i < NB_LIGNES; i++) {
        printf(ANSI_TEXTE_BLEU_FONCE "%d| " ANSI_RESET, i + 1);
        for (int j = 0; j < maxTour; j++) {
            if (waves[i][j] == '.') {
                printf(ANSI_BG_BLEU_MEGA_LIGHT ANSI_TEXTE_BLEU_MOYEN "%c  " ANSI_RESET, waves[i][j]);
            } else {
                printf(ANSI_BG_BLEU_FONCE ANSI_TEXTE_BLANC "%c" ANSI_BG_BLEU_MEGA_LIGHT "  " ANSI_RESET, waves[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}