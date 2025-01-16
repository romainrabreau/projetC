#include <assert.h>

#include "header.h"


// Fonction pour tester le tri et la résolution des doublons dans un fichier d'ennemis

void test_ResoudreFichier() {
    // Création d'un fichier temporaire
    const char* nom_fichier_test = "test_ennemis.txt";
    FILE* fichier_test = fopen(nom_fichier_test, "w+");
    assert(fichier_test != NULL);

    // Données de test
    fprintf(fichier_test, "3 2 Z\n");
    fprintf(fichier_test, "1 3 T\n");
    fprintf(fichier_test, "2 1 R\n");
    fprintf(fichier_test, "3 2 Z\n"); // Doublon à résoudre
    fprintf(fichier_test, "1 1 T\n");

    // Rewind pour préparer la lecture par ResoudreFichier
    rewind(fichier_test);

    // Appel de la fonction
    Erreur erreur;
    erreur.statut_erreur = 0;
    ResoudreFichier(fichier_test, &erreur);
    // Vérifie qu'il n'y a pas d'erreur
    assert(erreur.statut_erreur == 0);

    // Rewind pour lire les résultats
    rewind(fichier_test);

    // Vérifie le contenu trié et sans doublons
    char ligne[256];
    fgets(ligne, sizeof(ligne), fichier_test);
    assert(strcmp(ligne, "1 1 T\n") == 0);
    fgets(ligne, sizeof(ligne), fichier_test);
    assert(strcmp(ligne, "1 3 T\n") == 0); // Ligne attendue
    fgets(ligne, sizeof(ligne), fichier_test);
    assert(strcmp(ligne, "2 1 R\n") == 0); // Ligne attendue
    fgets(ligne, sizeof(ligne), fichier_test);
    assert(strcmp(ligne, "3 2 Z\n") == 0); // Ligne attendue
    fgets(ligne, sizeof(ligne), fichier_test);
    assert(strcmp(ligne, "4 2 Z\n") == 0); // Doublon décalé au tour suivant

    // Fermeture et suppression du fichier temporaire
    fclose(fichier_test);
    remove(nom_fichier_test);

    printf("Tests pour ResoudreFichier passés\n");
}

int main() {
    // Exécuter le test
    test_ResoudreFichier();
    return 0;
}
