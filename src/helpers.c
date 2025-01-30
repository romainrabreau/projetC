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

    // tableau alloué non dynamiquement, pas besoin de free
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

    int tour_courant = ennemis[0].tour;
    Etudiant bouges[NB_EMPLACEMENTS]; // tableau pour stocker les ennemis relégués
    int nb_bouges = 0;

    // gestion des doublons + écriture du nouveau fichier
    fprintf(fichier_ennemis, "%d %d %c\n", ennemis[0].tour, ennemis[0].ligne, ennemis[0].type);
    for (int i = 1; i < nbEnnemis; i++) {
            // Si on change de tour
        if (ennemis[i].tour > tour_courant) {
            // On écrit les repoussés qui sont pour ce tour
            for (int j = 0; j < nb_bouges; j++) {
                if (bouges[j].ligne < ennemis[i].ligne) {
                    fprintf(fichier_ennemis, "%d %d %c\n", bouges[j].tour, bouges[j].ligne, bouges[j].type);
                }
                else {
                    fprintf(fichier_ennemis, "%d %d %c\n", ennemis[i].tour, ennemis[i].ligne, ennemis[i].type);
                }
            }
            // On réinitialise la liste des redélégués au tour suivant
            nb_bouges = 0;
            tour_courant = ennemis[i].tour;
        }

        if (ennemis[i].tour == ennemis[i - 1].tour && ennemis[i].ligne == ennemis[i - 1].ligne) {
            ennemis[i].tour++; // décale le tour de l'ennemi suivant
            continue;
            }
        fprintf(fichier_ennemis, "%d %d %c\n", ennemis[i].tour, ennemis[i].ligne, ennemis[i].type);
    }
    // remet le curseur au début du fichier
    rewind(fichier_ennemis);
}

void Attendre(int ms) {
    usleep(ms * 1000);  // Pause en millisecondes
}

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
    Etudiant * prev_ennemi = jeu->etudiants;
    while (prev_ennemi) {
        if (prev_ennemi->ligne == e->ligne) {
            break;
        }
        prev_ennemi = prev_ennemi->next_line;
    }
    if (!prev_ennemi) {
        return;
    }

    Etudiant * devant = NULL;
    while (prev_ennemi && prev_ennemi->position < e->position) {
        devant = prev_ennemi;
        prev_ennemi = prev_ennemi->next_line;
    } 
    // - 'prev_ennemi' pointe vers le 1er ennemi ayant une position >= e->position
    //   OU bien prev_ennemi est NULL (fin de liste).
    // - 'devant' pointe vers l'ennemi qui a la position immédiatement derrière e (plus grande que e->position),
    //   OU  NULL si e doit s’insérer au tout début.

    // prev_ennemi est maintenant le l'ennemi le plus proche derrière
    // si les positions sont identiques, on décale de 1 tant que la position est occupée

    
    while (prev_ennemi && prev_ennemi->position == e->position && prev_ennemi->pointsDeVie >0) {
        printf("incrémentation de la position \n");
        e->position++;
        devant = prev_ennemi;
        prev_ennemi = prev_ennemi->next_line;
    }
    e->prev_line = devant;
    e->next_line = prev_ennemi;
    if (devant) {
        devant->next_line = e;
    }
    if (prev_ennemi) {
        prev_ennemi->prev_line = e;
    }
    printf("L'ennemi a sauté sur la ligne %d\n", e->ligne);
    printf("L'ennemi est à la position %d\n", e->position);
}