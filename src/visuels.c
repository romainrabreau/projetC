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

    printf("\t|");  // début de la barre
    for (int i = 0; i < LONG_BARRE; i++) {
        printf(" "); // au début, la barre est vide
    }
    printf("|"); 

    fflush(stdout); // envoie la sortie sur le terminal

    for (int i = 0; i <= LONG_BARRE; i++) {
        printf("\r\t|");  // retourne au début de la ligne
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

void AfficherTitre() {
    printf(ANSI_BG_BLEU_SHINY ANSI_TEXTE_BLANC"                ██╗     ██╗ ██████╗███████╗███╗   ██╗ ██████╗███████╗    ██╗   ██╗███████╗    ███████╗████████╗██╗   ██╗██████╗ ██╗ █████╗ ███╗   ██╗████████╗███████╗                \n");
    printf("                ██║     ██║██╔════╝██╔════╝████╗  ██║██╔════╝██╔════╝    ██║   ██║██╔════╝    ██╔════╝╚══██╔══╝██║   ██║██╔══██╗██║██╔══██╗████╗  ██║╚══██╔══╝██╔════╝                \n");
    printf("                ██║     ██║██║     █████╗  ██╔██╗ ██║██║     █████╗      ██║   ██║███████╗    █████╗     ██║   ██║   ██║██║  ██║██║███████║██╔██╗ ██║   ██║   ███████╗                \n");
    printf("                ██║     ██║██║     ██╔══╝  ██║╚██╗██║██║     ██╔══╝      ╚██╗ ██╔╝╚════██║    ██╔══╝     ██║   ██║   ██║██║  ██║██║██╔══██║██║╚██╗██║   ██║   ╚════██║                \n");
    printf("                ███████╗██║╚██████╗███████╗██║ ╚████║╚██████╗███████╗     ╚████╔╝ ███████║    ███████╗   ██║   ╚██████╔╝██████╔╝██║██║  ██║██║ ╚████║   ██║   ███████║                \n");
    printf("                ╚══════╝╚═╝ ╚═════╝╚══════╝╚═╝  ╚═══╝ ╚═════╝╚══════╝      ╚═══╝  ╚══════╝    ╚══════╝   ╚═╝    ╚═════╝ ╚═════╝ ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝                \n"ANSI_RESET);
    printf("\n");
}

int AfficherChoix(char options[][MAX_NAME_LEN], int n_options, Erreur *err) {
    // à partir d'un tableau d'options affiche un menu et renvoie le choix de l'utilisateur
    int choix = -1;

    while (1) {
        // calcul de la longueur maximale des options, pour aligner le menu
        int max_length = 0;
        for (int i = 0; i < n_options; ++i) {
            int len = strlen(options[i]) + 3; // 3 caractères de marge
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
            printf(ANSI_TEXTE_BLEU_FONCE "   %d. %s\n"ANSI_RESET, i + 1, options[i]);
        }

        // barrière du bas
        printf("\t\t\t\t\t\t\t---");
        for (int i = 0; i < max_length; i++) {
            printf("-");
        }
        printf("---\n\n");

        // Lecture du choix
        printf("\t\t\t\t\t\t\t"ANSI_BG_BLEU_MEGA_LIGHT ANSI_TEXTE_BLEU_MOYEN "Veuillez sélectionner une option (1-%d) : " ANSI_RESET, n_options);

        int entree;
        
        if (scanf("%d", &entree) != 1 || entree < 1 || entree > n_options) {
                printf("\t\t\t\t\t\t\tOption invalide. Veuillez entrer un nombre entre 1 et %d.\n", n_options);
                while (getchar() != '\n');
                
            }
        else {
            choix = entree - 1;
            break;
        }
    }
    return choix;
}


void VisualiserEnnemis(Etudiant* etudiants) {
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
        printf(ANSI_TEXTE_BLEU_MOYEN "%2d " ANSI_RESET, i + 1);
    }
    printf("\n");

    for (int i = 0; i <= maxTour; i++) {
        printf(ANSI_TEXTE_BLEU_MOYEN "---" ANSI_RESET);
    }
    printf("\n");
    
    for (int i = 0; i < NB_LIGNES; i++) {
        printf(ANSI_TEXTE_BLEU_MOYEN "%d| " ANSI_RESET, i + 1);
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