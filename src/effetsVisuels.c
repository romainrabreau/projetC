#include "header.h"
#define ANSI_WHITE_TEXT "\033[97m"
#define ANSI_BLUE_TEXT "\033[94m"

void animer_attente(int attente_ms, char *message) {
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

void print_avec_delai(const char *text, int delai_ms) {
    while (*text) {
        printf("%c", *text++);
        fflush(stdout);
        usleep(delai_ms * 1000);
    }
}

void AfficherTitre() {
    printf(ANSI_LIGHT_BLUE_BG ANSI_WHITE_TEXT"                ██╗     ██╗ ██████╗███████╗███╗   ██╗ ██████╗███████╗    ██╗   ██╗███████╗    ███████╗████████╗██╗   ██╗██████╗ ██╗ █████╗ ███╗   ██╗████████╗███████╗            \n");
    printf("                ██║     ██║██╔════╝██╔════╝████╗  ██║██╔════╝██╔════╝    ██║   ██║██╔════╝    ██╔════╝╚══██╔══╝██║   ██║██╔══██╗██║██╔══██╗████╗  ██║╚══██╔══╝██╔════╝            \n");
    printf("                ██║     ██║██║     █████╗  ██╔██╗ ██║██║     █████╗      ██║   ██║███████╗    █████╗     ██║   ██║   ██║██║  ██║██║███████║██╔██╗ ██║   ██║   ███████╗            \n");
    printf("                ██║     ██║██║     ██╔══╝  ██║╚██╗██║██║     ██╔══╝      ╚██╗ ██╔╝╚════██║    ██╔══╝     ██║   ██║   ██║██║  ██║██║██╔══██║██║╚██╗██║   ██║   ╚════██║            \n");
    printf("                ███████╗██║╚██████╗███████╗██║ ╚████║╚██████╗███████╗     ╚████╔╝ ███████║    ███████╗   ██║   ╚██████╔╝██████╔╝██║██║  ██║██║ ╚████║   ██║   ███████║            \n");
    printf("                ╚══════╝╚═╝ ╚═════╝╚══════╝╚═╝  ╚═══╝ ╚═════╝╚══════╝      ╚═══╝  ╚══════╝    ╚══════╝   ╚═╝    ╚═════╝ ╚═════╝ ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝            \n"ANSI_RESET);
}

void IntroMenu() {
    animer_attente(100, CLEAR_LINE);
    print_avec_delai(ANSI_BLUE_TEXT "Bienvenue dans 'Licence vs Étudiants'.\n" ANSI_RESET, 50);
    animer_attente(100, CLEAR_LINE);
    print_avec_delai(ANSI_BLUE_TEXT "Chargement du Menu... \n" ANSI_RESET, 50);
    animer_attente(200, CLEAR_LINE);
}

int AfficherChoix(char **options, int n_options) {
    int choice = -1;
    char input[10];

    while (1) {
        int max_length = 0;
        for (int i = 0; i < n_options; ++i) {
            int len = strlen(options[i]) + 3;
            if (len > max_length) {
                max_length = len;
            }
        }

        // Ligne du haut
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

        // Ligne du bas
        printf("\t\t\t\t\t\t\t---");
        for (int i = 0; i < max_length; i++) {
            printf("-");
        }
        printf("---\n\n");

        // Lecture du choix
        printf("\t\t\t\t\t\t\tVeuillez sélectionner une option (1-%d) : ", n_options);

        if (fgets(input, sizeof(input), stdin) != NULL) {
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') {
                input[len - 1] = '\0';  
                len--;
            }
            int selected = atoi(input);
            if (selected >= 1 && selected <= n_options) {
                choice = selected - 1;
                break;
            } else {
                fflush(stdout);
                printf("\n\t\t\t\t\t\t\tOption invalide. Veuillez entrer un nombre entre 1 et %d.\n", n_options);
                getchar(); 
            }
        } else {
            printf("\n\t\t\t\t\t\t\tErreur de lecture. Veuillez réessayer.\n");
        }
        printf("\n");
    }
    return choice; // renvoie le choix (0, 1, 2, etc.)
}

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