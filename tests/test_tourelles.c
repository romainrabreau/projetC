#include <assert.h>
#include "header.h"

void test_VerifEntreeLigne() {
    // Test 1: Entrée valide simple
    assert(VerifEntreeLigne("A 0") == 50);
    printf("Test 1 passé: Entrée valide simple\n");

    // Test 2: Plusieurs tourelles valides
    assert(VerifEntreeLigne("A 0, A 1") == 100);
    printf("Test 2 passé: Plusieurs tourelles valides\n");

    // Test 3: Espaces supplémentaires
    assert(VerifEntreeLigne("    A    0   ,    A    1    ") == 100);
    printf("Test 3 passé: Espaces supplémentaires\n");

    // Test 4: Symbole invalide
    assert(VerifEntreeLigne("B 0") == -1);
    printf("Test 4 passé: tourelle invalide\n");

    // Test 5: Position hors limites
    assert(VerifEntreeLigne("A 20") == -1);
    assert(VerifEntreeLigne("A -1") == -1);
    printf("Test 5 passé: Position hors limites\n");

    // Test 6: Position en double
    assert(VerifEntreeLigne("A 0, A 0") == -1);
    printf("Test 6 passé: Position en double\n");

    // Test 7: Format invalide
    assert(VerifEntreeLigne("A") == -1);
    assert(VerifEntreeLigne("A,0") == -1);
    assert(VerifEntreeLigne("A A 0") == -1);
    assert(VerifEntreeLigne(" 0 A") == -1);
    assert(VerifEntreeLigne(",") == -1);
    assert(VerifEntreeLigne(",,") == -1);
    printf("Test 7 passé: Format invalide\n");

    // Test 8: Virgules superflues
    assert(VerifEntreeLigne("A 0,") == 50);
    assert(VerifEntreeLigne("A 0,,A 1") == -1);
    printf("Test 8 passé: Virgules superflues\n");

    // Test 9: Caractères non-numériques pour la position
    assert(VerifEntreeLigne("A a") == -1);
    assert(VerifEntreeLigne("A #") == -1);
    printf("Test 9 passé: Caractères non-numériques\n");

    printf("Tous les tests ont passé avec succès!\n");
}

int main() {
    test_VerifEntreeLigne();
    return 0;
}