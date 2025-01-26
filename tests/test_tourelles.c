#include <assert.h>
#include "header.h"

void test_VerifEntreeLigne() {
    Erreur erreur;
    erreur.statut_erreur = 0;
    assert(VerifEntreeLigne("A 3\n", &erreur) == 50);

    assert(VerifEntreeLigne("A 7, A 1\n", &erreur) == 100);

    assert(VerifEntreeLigne("    A 12   ,    A 1   \n", &erreur) == 100);

    assert(VerifEntreeLigne("B 3", &erreur) == -1);

    // Position hors limites
    assert(VerifEntreeLigne("A 20", &erreur) == -1);
    assert(VerifEntreeLigne("A 0", &erreur) == -1);

    // Positions en double
    assert(VerifEntreeLigne("A 4, A 4", &erreur) == -1);

    assert(VerifEntreeLigne("A", &erreur) == -1);
    assert(VerifEntreeLigne("A,3", &erreur) == -1);
    assert(VerifEntreeLigne("A A9", &erreur) == -1);
    assert(VerifEntreeLigne("A10", &erreur) == -1);
    assert(VerifEntreeLigne(",A", &erreur) == -1);
    assert(VerifEntreeLigne(",,", &erreur) == -1);

    assert(VerifEntreeLigne("A 10,", &erreur) == 50);
    assert(VerifEntreeLigne("A 9,,A 1", &erreur) == -1);

    // caractères invalides
    assert(VerifEntreeLigne("A m", &erreur) == -1);

    printf("VerifEntreeLigne a été testée\n");
}

int main() {
    test_VerifEntreeLigne();
    return 0;
}