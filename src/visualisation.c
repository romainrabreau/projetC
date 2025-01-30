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

# define BAR_LENGTH 50
void barre_de_chargement(int ms) {
    int steps = BAR_LENGTH;
    int step_time = ms / steps;  // Time per step

    printf("|");  // Start of progress bar
    for (int i = 0; i < BAR_LENGTH; i++) {
        printf(" ");  // Empty spaces for the bar
    }
    printf("|");  // End bracket and initial percentage
    fflush(stdout);

    for (int i = 0; i <= BAR_LENGTH; i++) {
        printf("\r|");  // Return to start of line and print first bracket
        for (int j = 0; j < i; j++) {
            printf(ANSI_BG_BLEU_CLAIR " " ANSI_RESET);  // Filled block
        }
        for (int j = i; j < BAR_LENGTH; j++) {
            printf(" ");  // Empty space
        }
        printf("|");
        fflush(stdout);
        usleep(step_time * 1000);
    }

    printf("\n");  // Move to a new line after completion
}