#include "header.h"

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

# define LONG_BARRE 50
void barre_de_chargement(int ms) {
    int steps = LONG_BARRE;
    int step_time = ms / steps;  // Time per step

    printf("|");  // Start of progress bar
    for (int i = 0; i < LONG_BARRE; i++) {
        printf(" ");  // Empty spaces for the bar
    }
    printf("|");  // End bracket and initial percentage
    fflush(stdout);

    for (int i = 0; i <= LONG_BARRE; i++) {
        printf("\r|");  // Return to start of line and print first bracket
        for (int j = 0; j < i; j++) {
            printf(ANSI_BG_BLEU_CLAIR " " ANSI_RESET);  // Filled block
        }
        for (int j = i; j < LONG_BARRE; j++) {
            printf(" ");  // Empty space
        }
        printf("|");
        fflush(stdout);
        usleep(step_time * 1000);
    }
    printf("\n");  // Move to a new line after completion
}