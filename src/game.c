#include "header.h"

void ApparitionEnnemis(Jeu* jeu, Erreur* erreur) {
    Etudiant* e = jeu->etudiants;
    while (e != NULL) {
        if (e->tour == jeu->tour) {
            e->position = 14;
        }
        e = e->next;
    }
}

void ResoudreActionsTourelles(Jeu* jeu, Erreur* erreur) {
    // Exemple : chaque tourelle inflige 1 dégât à l’ennemi le plus proche (même ligne).
    Tourelle* t = jeu->tourelles;
    while (t != NULL) {
        int ligneTourelle = t->ligne;
        int posMin = NB_EMPLACEMENTS + 1;
        Etudiant* cible = NULL;

        Etudiant* e = jeu->etudiants;
        while (e != NULL) {
            if (e->ligne == ligneTourelle && e->pointsDeVie > 0 && e->position < posMin) {
                posMin = e->position;
                cible = e;
            }
            e = e->next;
        }

        if (cible) {
            // inflige 1 PV de dégât
            cible->pointsDeVie -= 1;
            if (cible->pointsDeVie <= 0) {
                // Ici, il faut prévoir la suppression de l'ennemi dans la liste
                // Par ex. une fonction supprimerEtudiant(Jeu* jeu, Etudiant* e)
            }
        }

        t = t->next;
    }
}

// ----------------------------------------------------------------------------
// 3) ResoudreActionsEnnemis : Les ennemis attaquent ou interagissent avec les tourelles
//    (ex: s'ils sont sur la même case qu'une tourelle, infliger des dégâts, etc.)
// ----------------------------------------------------------------------------
void ResoudreActionsEnnemis(Jeu* jeu, Erreur* erreur) {
    // Exemple : si un ennemi est sur la même (ligne, position) qu’une tourelle,
    // la tourelle perd 1 PV.
    Tourelle* t = jeu->tourelles;
    while (t != NULL) {
        Tourelle* t_next = t->next;  // pour éviter problème si on la supprime

        Etudiant* e = jeu->etudiants;
        while (e != NULL) {
            if (e->ligne == t->ligne && e->position == t->position && e->pointsDeVie > 0) {
                // la tourelle perd 1 PV
                t->pointsDeVie -= 1;
                if (t->pointsDeVie <= 0) {
                    // Prévoir la suppression de la tourelle si elle meurt
                    // ex: supprimerTourelle(Jeu* jeu, Tourelle* t).
                }
            }
            e = e->next;
        }

        t = t_next;
    }
}

// ----------------------------------------------------------------------------
// 4) DeplacerEnnemis : Les ennemis avancent (selon leur vitesse)
//    (ex: e->position += e->vitesse).
// ----------------------------------------------------------------------------
void DeplacerEnnemis(Jeu* jeu, Erreur* erreur) {
    Etudiant* e = jeu->etudiants;
    while (e != NULL) {
        e->position += e->vitesse;  // ex : +1, +2, etc.
        e = e->next;
    }
    // S'il y a un problème (ex : collisions impossibles), on peut setter l'erreur
}

// ----------------------------------------------------------------------------
// 5) VérifierDéfaite : Retourne 1 si un ennemi atteint la fin (université), 0 sinon
// ----------------------------------------------------------------------------
int VerifierDefaite(Jeu* jeu) {
    Etudiant* e = jeu->etudiants;
    while (e != NULL) {
        // Si la position dépasse NB_EMPLACEMENTS-1, c'est qu'il est arrivé au bout
        if (e->position >= NB_EMPLACEMENTS) {
            return 1; // défaite
        }
        e = e->next;
    }
    return 0; // pas de défaite
}

// ----------------------------------------------------------------------------
// 6) VérifierVictoire : Retourne 1 si tous les ennemis sont morts ou s'il n'y a plus d'ennemis
// ----------------------------------------------------------------------------
int VerifierVictoire(Jeu* jeu) {
    Etudiant* e = jeu->etudiants;
    int nbEnnemisEnVie = 0;
    while (e != NULL) {
        if (e->pointsDeVie > 0) {
            nbEnnemisEnVie++;
        }
        e = e->next;
    }
    if (nbEnnemisEnVie == 0) {
        return 1; // plus d’ennemis en vie => victoire
    }
    return 0;
}

// ----------------------------------------------------------------------------
// 7) JouerTour : Boucle principale décrite dans ton extrait
//    Incrémente le tour, applique les 5 étapes, vérifie fin de jeu.
// ----------------------------------------------------------------------------
void JouerTour(Jeu* jeu, Erreur* erreur) {
    // Dans ton exemple, c'était "while(18)" ; on met ici while(1) pour boucle infinie
    // (ou while(true) si tu disposes de <stdbool.h>).
    while (1) {
        jeu->tour++;
        printf("Tour %d\n", jeu->tour);

        // Étape 1
        ApparitionEnnemis(jeu, erreur);
        if (erreur->statut_erreur == 1) return;

        // Étape 2
        ResoudreActionsTourelles(jeu, erreur);
        if (erreur->statut_erreur == 1) return;

        // Étape 3
        ResoudreActionsEnnemis(jeu, erreur);
        if (erreur->statut_erreur == 1) return;

        // Étape 4
        DeplacerEnnemis(jeu, erreur);
        if (erreur->statut_erreur == 1) return;

        // Étape 5
        if (VerifierDefaite(jeu)) {
            printf("Vous avez perdu ... Les étudiants ont pris le contrôle de l'université.\n");
            break;
        }
        if (VerifierVictoire(jeu)) {
            printf("Victoire, bien joué. Vous avez défendu l'université.\n");
            break;
        }

        // Petite pause : on demande à l'utilisateur d'appuyer sur Entrée
        printf("Appuyez sur Entrée pour continuer...\n");
        // Vider le \n restant
        while (getchar() != '\n');
        // Attendre un appui
        getchar();
    }
}

// ----------------------------------------------------------------------------
// 8) (Optionnel) VisualiserEnnemis ou VisualiserJeu : affichage de la grille en ASCII
//    Tu peux séparer ça dans un interface.c, ou le laisser ici si tu préfères
// ----------------------------------------------------------------------------
void VisualiserEnnemis(Etudiant* liste, Erreur* erreur) {
    // Ex. simple : afficher juste la liste d’ennemis
    // Dans un vrai tower defense, tu peux faire un affichage 2D.
    printf("\033[0;0H"); 
    printf("\033[2J");

    printf("Vagues : \n");
    Etudiant* e = liste;
    while (e != NULL) {
        printf("Ennemi type '%c' => Tour: %d, Ligne: %d, Pos: %d, PV: %d\n",
               (char)(e->type), e->tour, e->ligne, e->position, e->pointsDeVie);
        e = e->next;
    }
    // Si tu veux manipuler l'erreur en cas de soucis, tu peux faire :
    // erreur->statut_erreur = 1;
    // strcpy(erreur->msg_erreur, "Blabla...");
}
