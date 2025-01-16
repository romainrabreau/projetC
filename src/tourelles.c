#include "header.h"


const TypeTourelle TYPES_TOURELLES[] = {
    // symbole, points de vie, prix, nom
    {'A', 5, 50, "Tableau noir"},      // Type de base

};

Tourelle * InitialiserTourelles(int * cagnotte, Erreur* erreur){
    printf("Voici les tourelles disponibles ainsi que leurs caractéristiques :\n");
    for (int i = 0; i < NB_TYPES_TOURELLES; i++) {
        printf("%c : %s\n", TYPES_TOURELLES[i].symbole, TYPES_TOURELLES[i].nom);
        printf("Points de vie : %d\n", TYPES_TOURELLES[i].pointsDeVie);
        printf("Prix : %d\n", TYPES_TOURELLES[i].prix);
    }
    //TODO si on ajoute des tourelles spéciales
    printf("Certaines tourelles ont des caractéristiques spéciales, notamment : \n");
    
    printf("Il vous faut maintenant placer les tourellles de défense sur les emplacements de votre choix.\n");
    printf("Vous avez à défendre %d lignes, avec %d positions par ligne \n", NB_LIGNES, NB_EMPLACEMENTS);

    printf("Si vous souhaitez placer des tourelles sur la ligne proposée, entrez [SYMBOLE_X] [EMPLACEMENT_1], [SYMBOLE_Y] [EMPLACEMENT_2], ...\n");
    printf("Sinon, faites entrée pour passer à la ligne suivante\n");
    printf("SYMBOLE_X doit être un symbole de tourelle valide, et EMPLACEMENT_i doit être un entier entre 0 et %d\n", NB_EMPLACEMENTS-1);
    printf("Vous ne pouvez pas placer deux tourelles sur le même emplacement\n");
    printf("Vous ne pouvez pas dépenser plus que votre cagnotte.\n");
    Tourelle* premier = NULL;
    int sortie_verif;
    for (int i = 0; i < NB_LIGNES; i++) {
        printf("Il vous reste %d ECTS à dépenser.\n", *cagnotte);
        printf("Ligne %d : ", i);
        char ligne_tourelles[256];
        fgets(ligne_tourelles, sizeof(ligne_tourelles), stdin);
        if (ligne_tourelles[0] == '\n') {
            continue;
        }
        sortie_verif = VerifEntreeLigne(ligne_tourelles, cagnotte);
        if (sortie_verif == -1) {
            printf("Votre entrée est invalide, veuillez re-placer les tourelles en respectant le format\n");
            i--;
            continue;
        }
        if (sortie_verif > 0) {
            printf("Vous avez dépassé le solde de votre cagnotte de %d ECTS, veuillez re-placer les tourelles\n", sortie_verif);
            i--;
            continue;
        }
        else {
            //TODO
        }
    }

}

void LibererTourelles(Tourelle* premier) {
    while (premier != NULL) {
        Tourelle* temp = premier;
        premier = premier->next;
        free(temp);
    }
}

int VerifEntreeLigne(char * ligne_tourelles, const int * cagnotte) {
    /* vérifie 
    1. La validité des symboles de tourelles
    2. La validité du numéro de la position (entre 0 et NB_EMPLACEMENTS-1)
    3. Pas de doublons dans les positions
    4. Les paires sont symbole, position
    5. Pas de dépassement de la cagnotte

    Retourne le nombre d'ECTS dépassés
    */
    int positions[NB_EMPLACEMENTS] = {0};  // Pour marquer les positions utilisées
    int solde_courant = 0;
    char symbole;
    int position;
    char* ptr = ligne_tourelles;
    
    // On lit deux par deux : symbole puis position
    while (sscanf(ptr, " %c %d", &symbole, &position) == 2) {
        // Vérifie le symbole
        const TypeTourelle* type = NULL;
        for (int i = 0; i < NB_TYPES_TOURELLES; i++) {
            if (TYPES_TOURELLES[i].symbole == symbole) {
                type = &TYPES_TOURELLES[i];
                break;
            }
        }
        if (!type) return -1;  // Symbole invalide
        
        // Vérifie la position
        if (position < 0 || position >= NB_EMPLACEMENTS) return -1;
        if (positions[position]) return -1;  // Position déjà utilisée
        positions[position] = 1;
        
        // Vérifie le coût
        solde_courant += type->prix;
        if (solde_courant > *cagnotte) {
            return solde_courant - *cagnotte;  // Retourne le dépassement
        }
        
        // Avance au prochain couple symbole-position
        // On cherche la virgule ou la fin de ligne
        while (*ptr && *ptr != ',' && *ptr != '\n') ptr++;
        if (*ptr == ',') ptr++;
    }
    
    return 0;  // Tout est valide
}