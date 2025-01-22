#include "header.h"

void AfficherPlateau(Jeu* jeu) {
    printf("\033[0;0H");
    printf("\033[2J");

    // Affiche le tour
    printf("Tour %d\n", jeu->tour);

    // Tableau tampon pour chaque case, ici on stocke 4 caractères max (ex: "3Z\0")
    // Par défaut, on met "." pour indiquer vide.
    char plateau[NB_LIGNES][NB_EMPLACEMENTS][4];
    for(int i=0; i<NB_LIGNES; i++){
        for(int j=0; j<NB_EMPLACEMENTS; j++){
            plateau[i][j][0] = '.';
            plateau[i][j][1] = '\0'; // fin de chaîne
        }
    }

    // Place les tourelles (“T”)
    // (On suppose qu’une case ne contient qu’une tourelle ou un ennemi
    //  si le jeu permet plusieurs occupants, il faudra gérer autrement)
    Tourelle* t = jeu->tourelles;
    while(t){
        int li = t->ligne;     // de 0 à NB_LIGNES-1
        int pos = t->position; // 0..NB_EMPLACEMENTS-1
        if(li>=0 && li<NB_LIGNES && pos>=0 && pos<NB_EMPLACEMENTS){
            // Stocke simplement "T"
            plateau[li][pos][0] = 'T';
            plateau[li][pos][1] = '\0';
        }
        t = t->next;
    }

    // Place les ennemis, ex: “3Z” si PV=3 et type='Z'
    Etudiant* e = jeu->etudiants;
    while(e){
        // Dans cet exemple, on suppose e->ligne varie de 1..7, on fait -1 pour index
        int li = e->ligne - 1; 
        int pos = e->position;
        if(li>=0 && li<NB_LIGNES && pos>=0 && pos<NB_EMPLACEMENTS && e->pointsDeVie>0){
            // On formate “PVType”, ex: “3Z”
            //  On se limite à 3 caractères max (“3Z\0”)
            snprintf(plateau[li][pos], 4, "%d%c", e->pointsDeVie, (char)e->type);
        }
        e = e->next;
    }

    // Affichage final
    for(int i=0; i<NB_LIGNES; i++){
        // Numéro de ligne, ex. “1| “
        // i+1 car l’énoncé utilise 1..7 visuellement
        printf("%d| ", i+1); 
        for(int j=0; j<NB_EMPLACEMENTS; j++){
            // On centre un peu (3 ou 4 caractères), ex:
            // “3Z “ ou “T  ” ou “.  ”
            printf("%-3s", plateau[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
