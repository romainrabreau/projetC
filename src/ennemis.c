#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Rappel de la structure TypeEnnemi :

  typedef struct {
      const char symbole; // Caractère représentant le type
      int pointsDeVie;
      int degats;
      int vitesse;
      const char* nom;    // Nom du type
  } TypeEnnemi; 
*/

// Types d'ennemis (symbole, pointsDeVie, degats, vitesse, nom)
const TypeEnnemi TYPES_ENNEMIS[] = {
    { 'Z', 5, 1, 1,   "Etudiant"          },
    { 'T', 3, 2, 1,   "Etudiant Talent"   },
    { 'L', 8, 1, 1,   "Etudiant L1"       },
    { 'A', 0, 0, 2,   "Alternant"         } 
};

const TypeEnnemi* trouverTypeEnnemi(char symbole) {
    for (int i = 0; i < NB_TYPES_ENNEMIS; i++) {
        if (TYPES_ENNEMIS[i].symbole == symbole) {
            return &TYPES_ENNEMIS[i];
        }
    }
    return NULL;
}

//Vérifie la validité d'un symbole, renvoie le TypeEnnemi*.
const TypeEnnemi* VerifType(int *tour, int *ligne, char *symbole, Erreur *erreur) {
    if (!symbole || !erreur) return NULL;
    const TypeEnnemi* type_ennemi = trouverTypeEnnemi(*symbole);
    if (!type_ennemi) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "type d'ennemi invalide\n");
        return NULL;
    }
    return type_ennemi;
}

/*
 *  InitialiserEnnemis
 *  - Lit le fichier contenant [tour ligne symbole]
 *  - Alloue et chaîne les ennemis (premier->next->...).
 *  - Chaînage supplémentaire par ligne : next_line, prev_line
 */
Etudiant* InitialiserEnnemis(FILE* fichier_ennemis, Jeu* jeu, Erreur* erreur) {
    if (!fichier_ennemis) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Le fichier n'a pas pu être ouvert\n");
        return NULL;
    }
    if (!jeu || !erreur) {
        fprintf(stderr, "Paramètres invalides à InitialiserEnnemis.\n");
        return NULL;
    }

    Etudiant* premier = NULL;
    Etudiant* dernier = NULL;
    Etudiant* lignes_ennemis[NB_LIGNES];
    memset(lignes_ennemis, 0, sizeof(lignes_ennemis));

    char ligne_fichier[256];

    while (fgets(ligne_fichier, sizeof(ligne_fichier), fichier_ennemis)) {
        int tour, num_ligne;
        char symbole_type;
        // On attend 3 infos : tour, ligne, symbole
        if (sscanf(ligne_fichier, "%d %d %c", &tour, &num_ligne, &symbole_type) != 3) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "format de ligne invalide (attendu: 'tour ligne symbole')\n");
            LibererEnnemis(premier);
            return NULL;
        }
        // Vérifie la validité du symbole
        const TypeEnnemi* type_ennemi = VerifType(&tour, &num_ligne, &symbole_type, erreur);
        if (erreur->statut_erreur == 1) {
            LibererEnnemis(premier);
            return NULL;
        }

        // Alloue un nouvel étudiant
        Etudiant* nouvel = (Etudiant*)malloc(sizeof(Etudiant));
        if (!nouvel) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "erreur d'allocation mémoire (Etudiant)\n");
            LibererEnnemis(premier);
            return NULL;
        }

        // Initialisation des champs
        nouvel->ligne = num_ligne;
        nouvel->tour  = tour;
        nouvel->position = -1;  // par défaut pas encore sur le plateau
        nouvel->next = NULL;
        nouvel->next_line = NULL;
        nouvel->prev_line = NULL;

        // Copie du type
        nouvel->type = (int)type_ennemi->symbole; 
        nouvel->pointsDeVie = type_ennemi->pointsDeVie;
        nouvel->vitesse     = type_ennemi->vitesse;

        // Chaînage dans la liste globale
        if (!premier) {
            premier = nouvel;
        } else {
            dernier->next = nouvel;
        }
        dernier = nouvel;

        // Chaînage par ligne
        int idx = num_ligne - 1;
        if (idx < 0 || idx >= NB_LIGNES) {
            // ligne invalide => on peut lever une erreur ou ignorer
            // Ici, on génère une erreur
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "ligne hors borne\n");
            LibererEnnemis(premier);
            return NULL;
        }
        if (!lignes_ennemis[idx]) {
            lignes_ennemis[idx] = nouvel;
        } else {
            // on se chaîne en fin de la sous-liste
            Etudiant* tmp = lignes_ennemis[idx];
            // se place en fin
            while (tmp->next_line) {
                tmp = tmp->next_line;
            }
            tmp->next_line = nouvel;
            nouvel->prev_line = tmp;
            // on peut actualiser lignes_ennemis[idx] si on veut pointer sur le dernier
            lignes_ennemis[idx] = nouvel;
        }
    }

    return premier;
}

void supprimerEtudiant(Jeu *jeu, Etudiant *e)
{
    if (!jeu || !e) return;

    // 1) Retirer l'ennemi de la liste chaînée globale
    Etudiant *current = jeu->etudiants;
    Etudiant *prev    = NULL;

    while (current) {
        if (current == e) {
            // Si c'est le premier élément
            if (prev == NULL) {
                jeu->etudiants = current->next;
            }
            else {
                prev->next = current->next;
            }
            break;  // On sort du while, mais on continue à gérer next_line/prev_line
        }
        prev    = current;
        current = current->next;
    }

    // 2) Retirer l'ennemi de la sous-liste par ligne
    //    e->prev_line <-> e <-> e->next_line
    if (e->prev_line) {
        e->prev_line->next_line = e->next_line;
    }
    if (e->next_line) {
        e->next_line->prev_line = e->prev_line;
    }

    // 3) Libérer la mémoire
    free(e);
}


/*
 *  LibererEnnemis
 *  - Libère la liste chaînée (next) d'Etudiant.
 */
void LibererEnnemis(Etudiant* premier) {
    while (premier) {
        Etudiant* courant = premier;
        premier = premier->next;
        free(courant);
    }
}
