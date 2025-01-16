#include "header.h"


const TypeEnnemi TYPES_ENNEMIS[] = {
    {'Z', 5, 1, "Zombie"},      // Type de base
    {'R', 3, 2, "Runner"},      // Plus rapide mais moins résistant
    {'T', 8, 1, "Tank"}         // Plus résistant mais même vitesse
};

// retourne le type d'ennemi correspondant au symbole
const TypeEnnemi* trouverType(char symbole) {
    for(int i = 0; i < NB_TYPES_ENNEMIS; i++) {
        if(TYPES_ENNEMIS[i].symbole == symbole) {
            return &TYPES_ENNEMIS[i];
        }
    }
    return NULL;
}

void InitialiserEnnemiType(Etudiant* etudiant, const TypeEnnemi* type) {
    etudiant->pointsDeVie = type->pointsDeVie;
    etudiant->vitesse = type->vitesse;
    etudiant->type = type->symbole;
}

// on considère que chaque élement dans le fichier d'apparition d'un ennemi est unique et triée par ordre croissant
Etudiant* InitialisationEnnemis(FILE* fichier_ennemis, Jeu* jeu, Erreur* erreur) {
    if (fichier_ennemis == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "le fichier n'a pas pu être ouvert\n");
        return NULL;
    }

    Etudiant* premier = NULL;  // premier ajouté
    Etudiant* dernier = NULL;  // dernier ajouté
    Etudiant* lignes_ennemis[NB_LIGNES] = {NULL}; // dernier ajouté par ligne

    char ligne_fichier[256];

    while (fgets(ligne_fichier, sizeof(ligne_fichier), fichier_ennemis)) {
        int tour, ligne;
        char symbole_type;
        if (sscanf(ligne_fichier, "%d %d %c", &tour, &ligne, &symbole_type) != 3) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "format de ligne invalide\n");
            return NULL;
        }
        // obtenir type d'ennemi à partir du symbole correspondant
        TypeEnnemi* type = VerifType(&tour, &ligne, &symbole_type, erreur);
        if (erreur->statut_erreur == 1) {
            LibererMemEnnemis(premier);
            return NULL;
        }

        // nouvel étudiant ennemi
        Etudiant* nouvel_etudiant = (Etudiant*)malloc(sizeof(Etudiant));
        if (nouvel_etudiant == NULL) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "erreur d'allocation mémoire\n");
            LibererEnnemis(premier);
            return NULL;
        }

        // Initialisation des champs
        nouvel_etudiant->ligne = ligne;
        nouvel_etudiant->tour = tour;
        nouvel_etudiant->next = NULL;
        nouvel_etudiant->next_line = NULL;
        nouvel_etudiant->prev_line = NULL;
        InitialiserEnnemiType(nouvel_etudiant, type);

        // ajout à la fin de la liste principale
        if (premier == NULL) {
            premier = nouvel_etudiant;
        } else {
            dernier->next = nouvel_etudiant;
        }
        dernier = nouvel_etudiant;

        // chaînage par ligne
        if (lignes_ennemis[ligne - 1] == NULL) {
            lignes_ennemis[ligne - 1] = nouvel_etudiant;
        } else {
            lignes_ennemis[ligne - 1]->next_line = nouvel_etudiant;
            nouvel_etudiant->prev_line = lignes_ennemis[ligne - 1];
            // on met à jour la référence au dernier ennemi de la ligne de jeu
            lignes_ennemis[ligne - 1] = nouvel_etudiant;
        }
    }

    return premier;
}


// vérifie que le type est dans les types autorisés
TypeEnnemi* VerifType(int *tour, int *ligne, char *type, Erreur *erreur) {
    const TypeEnnemi* type_ennemi = trouverType(type);
    if (type_ennemi == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "type d'ennemi invalide\n");
        return NULL;
    }
    return type_ennemi;
}

// libère dynamiquement la mémoire allouée pour les ennemis
void LibererEnnemis(Etudiant* premier) {
    Etudiant* courant = premier;
    while (courant != NULL) {
        Etudiant* suivant = courant->next;
        free(courant);
        courant = suivant;
    }
}