#include "header.h"


/* POSSIBLEMENT INUTILE, JSP


void test_InitEnnemis() {
    const char* nom_fichier_test = "test_ennemi.txt";
    FILE* fichier_test = fopen(nom_fichier_test, "w+");
    assert(fichier_test != NULL);
    fprintf(fichier_test, "1 1 Z\n2 2 R\n3 3 T\n");

    Jeu jeu;
    Erreur erreur;
    erreur.statut_erreur = 0;
    Etudiant* premier = InitialisationEnnemis(fichier_test, &jeu, &erreur);

    assert(erreur.statut_erreur == 0);
    assert(premier != NULL);

    // vérifie la liste chaînée
    Etudiant* current = premier;
    assert(current->tour == 1 && current->ligne == 1 && current->type == 'Z');
    current = current->next;
    assert(current->tour == 2 && current->ligne == 2 && current->type == 'R');
    current = current->next;
    assert(current->tour == 3 && current->ligne == 3 && current->type == 'T');
    assert(current->next == NULL);

    // supprime les instances crées
    while (premier) {
        Etudiant* temp = premier;
        premier = premier->next;
        free(temp);
    }
    fclose(fichier_test);
    remove(nom_fichier_test);
}*/

void test_EnnemiInvalid() {
    const char* nom_fichier_test = "test_ennemi_invalide.txt";
    FILE* fichier_test = fopen(nom_fichier_test, "w+");
    assert(fichier_test != NULL);
    fprintf(fichier_test, "1 1 X\n");

    Jeu jeu;
    Erreur erreur;
    erreur.statut_erreur = 0;
    Etudiant* premier = InitialisationEnnemis(fichier_test, &jeu, &erreur);

    assert(premier == NULL);
    assert(erreur.statut_erreur == 1);
    assert(strcmp(erreur.msg_erreur, "type d'ennemi invalide\n") == 0);

    fclose(fichier_test);
    remove(nom_fichier_test);
}