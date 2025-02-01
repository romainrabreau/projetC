#include "header.h"


void test_EnnemiInvalide() {
    const char* nom_fichier_test = "test_ennemi_invalide.txt";
    FILE* fichier_test = fopen(nom_fichier_test, "w+");
    assert(fichier_test != NULL);
    fprintf(fichier_test, "1 1 H\n");

    Jeu jeu;
    Erreur erreur;
    erreur.statut_erreur = 0;
    Etudiant* premier = InitialisationEnnemis(fichier_test, &jeu, &erreur);

    assert(premier == NULL);
    assert(erreur.statut_erreur == 1);
    assert(strcmp(erreur.msg_erreur, "type d'ennemi invalide\n") == 0);

    fclose(fichier_test);
    remove(nom_fichier_test);
    printf("Test passés pour InitialisationEnnemis avec un fichier test");
}

int main() {
    test_EnnemiInvalide();
    return 0;
}