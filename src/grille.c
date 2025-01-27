#include "header.h"

void AfficherPlateau(Jeu* jeu) {
    // Tableau tampon pour chaque case, ici on stocke 4 caractères max (ex: "3Z\0")
    char plateau[NB_LIGNES][NB_EMPLACEMENTS + 1][4]; // Emplacement supplémentaire pour les zombies arrivant

    for(int i = 0; i < NB_LIGNES; i++){
        for(int j = 0; j < NB_EMPLACEMENTS + 1; j++){
            if (j < NB_EMPLACEMENTS) {
                plateau[i][j][0] = '.';
                plateau[i][j][1] = '\0'; // Fin de chaîne
            } else {
                plateau[i][j][0] = ' ';
                plateau[i][j][1] = '\0';
            }
        }
    }
    // (On suppose qu’une case ne contient qu’une tourelle ou un ennemi)
    Tourelle* t = jeu->tourelles;
    while(t){
        int li = t->ligne-1;     // de 0 à NB_LIGNES-1
        int pos = t->position-1; // 0..NB_EMPLACEMENTS-1
        if(li>=0 && li<NB_LIGNES && pos>=0 && pos<NB_EMPLACEMENTS && t->pointsDeVie>0){
            snprintf(plateau[li][pos], 4, "%d%c", t->pointsDeVie, (char)t->type);
        }
        t = t->next;
    }

    // Place les ennemis, ex: “3Z” si PV=3 et type='Z'
    Etudiant* e = jeu->etudiants;
    while(e){
        // Dans cet exemple, on suppose e->ligne varie de 1..7, on fait -1 pour index
        int li = e->ligne-1; 
        int pos = e->position-1;
        if(li>=0 && li<NB_LIGNES && pos>=0 && pos < NB_EMPLACEMENTS + 1 && e->pointsDeVie>0){
            snprintf(plateau[li][pos], 4, "%d%c", e->pointsDeVie, (char)e->type);
        }
        e = e->next;
    }

printf("   ");
    for (int j = 0; j < NB_EMPLACEMENTS; j++) {
        // Largeur 3 par colonne, alignée à gauche
        printf("%-3d", j + 1);
    }
    printf("\n");

    for (int j = 0; j < NB_EMPLACEMENTS + 1; j++) {
        printf("---");
    }
    printf("\n");

    // Affichage final
    for(int i=0; i<NB_LIGNES; i++){
        printf("%d| ", i+1);        // Numéro de ligne, ex. “1| “
        for(int j=0; j < NB_EMPLACEMENTS + 1; j++){
            printf("%-3s", plateau[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}