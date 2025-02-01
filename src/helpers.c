#include "header.h"

void ResoudreFichier(FILE* fichier_ennemis, Erreur* erreur) {
    if (fichier_ennemis == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "fichier invalide\n");
        return;
    }

    // tableau alloué non dynamiquement, pas besoin de free
    
    int ligne_courante = 0;
    int tour_courant = 0;

    char ligne_fichier[256];
    // boucle pour lire les lignes du fichier 
    while (fgets(ligne_fichier, sizeof(ligne_fichier), fichier_ennemis)) {
        int tour, ligne;
        char type;
        if (sscanf(ligne_fichier, "%d %d %c", &tour, &ligne, &type) != 3) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "format de ligne invalide\n");
            return;
        }

        if (ligne < 1 || ligne > NB_LIGNES || tour < 0){
            erreur->statut_erreur = 1;
            sprintf(erreur->msg_erreur, "le tour ou la ligne entré(e) est invalide : %d\n", ligne);
            return;
        }
        if (tour < tour_courant) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "l'apparition des ennemis n'est pas triée par tour\n");
            return;
        }
        if (tour == tour_courant && ligne <= ligne_courante) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "l'apparition des ennemis n'est pas triée par ligne\n");
            return;
        }
        tour_courant = tour;
        ligne_courante = ligne;
    }

    // remet le curseur au début du fichier
    rewind(fichier_ennemis);
    fgets(ligne_fichier, sizeof(ligne_fichier), fichier_ennemis);  // skip une ligne (la cagnotte)
    printf("Le fichier est trié\n");
}




// ne fonctionne pas
void ChangerLigne(Jeu * jeu, Etudiant* e, int saut) {
    printf("L'ennemi est à la position %d\n", e->position);
    if (e == NULL || jeu == NULL || saut == 0) {
        return;
    }
    // change la ligne de l'ennemi
    e->ligne += saut;
    // modifie le chaînage de la ligne d'origine
    if (e->prev_line) {
        e->prev_line->next_line = e->next_line;
    }
    if (e->next_line) {
        e->next_line->prev_line = e->prev_line;
    }
    e->prev_line = NULL;
    e->next_line = NULL;
    //place l'ennemi 
    Etudiant * current = jeu->etudiants;
    while (current) {
        if (current->ligne == e->ligne) {
            break;
        }
        current = current->next;
    }
    printf("current est à la position %d\n", current->position);
    printf("current est à la ligne %d\n", current->ligne);
    printf("current est de type %c\n", (char)current->type);
    if (!current) {
        printf("il est seul sur la ligne %d\n", e->ligne);
        printf(" est à la position %d\n", e->position);
        return;
    }

    Etudiant * devant = NULL;
    while (current && current->position < e->position) {
        printf("prochain ennemi : %d\n", current->position);
        devant = current;
        current = current->next_line;
    } 
    // - 'prev_ennemi' pointe vers le 1er ennemi ayant une position >= e->position
    //   OU bien prev_ennemi est NULL (fin de liste).
    // - 'devant' pointe vers l'ennemi qui a la position immédiatement derrière e (plus grande que e->position),
    //   OU  NULL si e doit s’insérer au tout début.

    // prev_ennemi est maintenant le l'ennemi le plus proche derrière
    // si les positions sont identiques, on décale de 1 tant que la position est occupée

    printf("current est à la position %d\n", current->position);
    if (devant) {
        printf("devant est à la position %d\n", devant->position);
    }
    while (current && current != e && current->position == e->position && current->pointsDeVie >0) {
        printf("incrémentation de la position \n");
        printf("prochaine ennemi type : %c\n", (char)current->type);
        e->position++;
        devant = current;
        current = current->next_line;
    }

    e->prev_line = devant;
    e->next_line = current;


    if (devant) {
        devant->next_line = e;
    }
    if (current) {
        current->prev_line = e;
    }
    printf("L'ennemi a sauté sur la ligne %d\n", e->ligne);
    printf("L'ennemi est à la position %d\n", e->position);
}