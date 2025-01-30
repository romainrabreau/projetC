#include "header.h"
// fonction de tri pour qsort
int comparerEnnemis(const void* a, const void* b) {
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

    int cagnotte = 0; // la cagnotte n'était pas lue donc ça buguait
    char newline;
    if (fscanf(fichier_ennemis, "%d%c", &cagnotte, &newline) != 2 || newline != '\n') {
        // Erreur si on ne lit pas un entier suivi d’un saut de ligne
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Cagnotte introuvable ou invalide\n");
        return;
    }

    // tableau alloué non dynamiquement, pas besoin de free
    Etudiant ennemis[NB_ENNEMIS_MAX];
    int nbEnnemis = 0;
    char ligne_fichier[256];

    while (fgets(ligne_fichier, sizeof(ligne_fichier), fichier_ennemis)) {
        int tour, ligne;
        char type;
        if (sscanf(ligne_fichier, "%d %d %c", &tour, &ligne, &type) != 3) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "format de ligne invalide\n");
            fclose(fichier_ennemis);
            return;
        }
        if (tour < 0 || ligne < 1 || ligne > NB_LIGNES) {
            erreur->statut_erreur = 1;
            sprintf(erreur->msg_erreur,
                    "Tour/ligne invalide (tour=%d, ligne=%d)\n", tour, ligne);
            fclose(fichier_ennemis);
            return;
        }
        ennemis[nbEnnemis].tour  = tour;
        ennemis[nbEnnemis].ligne = ligne;
        ennemis[nbEnnemis].type  = type;
        nbEnnemis++;
        if (nbEnnemis >= NB_ENNEMIS_MAX) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Trop d'ennemis\n");
            fclose(fichier_ennemis);
            return;
        }
    }

    // Tri initial
    qsort(ennemis, nbEnnemis, sizeof(Etudiant), comparerEnnemis);

    // Résolution des collisions : on incrémente le 'tour' de l'ennemi suivant s'il y a collision
    if (nbEnnemis > 1) {
        int collision;
        do {
            collision = 0;
            for (int i = 0; i < nbEnnemis - 1; i++) {
                for (int j = i + 1; j < nbEnnemis; j++) {
                    if (ennemis[i].tour == ennemis[j].tour &&
                        ennemis[i].ligne == ennemis[j].ligne)
                    {
                        ennemis[j].tour++;  // On repousse le tour
                        collision = 1;
                    }
                }
            }
            if (collision) {
                qsort(ennemis, nbEnnemis, sizeof(Etudiant), comparerEnnemis);
            }
        } while (collision);
    }

    // On réécrit depuis le début
    rewind(fichier_ennemis);
    for (int i = 0; i < nbEnnemis; i++) {
        fprintf(fichier_ennemis, "%d %d %c\n",
                ennemis[i].tour, ennemis[i].ligne, ennemis[i].type);
    }

    fflush(fichier_ennemis);
    fclose(fichier_ennemis);
}

void Attendre(int ms) {
    usleep(ms * 1000);  // Pause en millisecondes
}