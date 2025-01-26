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