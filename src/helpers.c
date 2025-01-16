#include "header.h"

// fonction de tri pour qsort
int comparerEnnemis(const void* a, const void* b) {
    // qsort renvoi deux pointeurs void, on les cast en Etudiant
    const Etudiant* ennemiA = (const Etudiant*)a;
    const Etudiant* ennemiB = (const Etudiant*)b;
    if (ennemiA->tour != ennemiB->tour) {
        return ennemiA->tour - ennemiB->tour;
    }
    return ennemiA->ligne - ennemiB->ligne;
}

void ResoudreFichier(FILE* fichier_ennemis, Erreur* erreur) {
    if (fichier_ennemis == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "fichier invalide\n");
        return;
    }

    // tableau alloué sur la pile, pas besoin de free
    Etudiant ennemis[NB_ENNEMIS_MAX];
    int nbEnnemis = 0;

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

        ennemis[nbEnnemis].tour = tour;
        ennemis[nbEnnemis].ligne = ligne;
        ennemis[nbEnnemis].type = type;
        nbEnnemis++;
    }

    // tri des ennemis en fonction du tour puis de la ligne
    qsort(ennemis, nbEnnemis, sizeof(Etudiant), comparerEnnemis);

    rewind(fichier_ennemis);
    // gestion des doublons + écriture du nouveau fichier
    fprintf(fichier_ennemis, "%d %d %c\n", ennemis[0].tour, ennemis[0].ligne, ennemis[0].type);
    
    for (int i = 1; i < nbEnnemis; i++) {
        if (ennemis[i].tour == ennemis[i - 1].tour && ennemis[i].ligne == ennemis[i - 1].ligne) {
            ennemis[i].tour++; // décale le tour de l'ennemi suivant
        }
        fprintf(fichier_ennemis, "%d %d %c\n", ennemis[i].tour, ennemis[i].ligne, ennemis[i].type);
    }
    // remet le curseur au début du fichier
    rewind(fichier_ennemis);
}
