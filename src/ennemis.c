#include "header.h"

/*
à ajouter : 
type Insomniaque -  I sa vitesse augmente à chaque tour
type Syndicaliste - S augmente la vitesse des ennemis adjacents lorsqu'il est touché
type Fainéant - F fait des sauts aléatoires ou ne bouge pas pendant plusieurs tour, forte résistance
*/

const TypeEnnemi TYPES_ENNEMIS[] = {
    // symbole, points de vie, vitesse, nom
    {'Z', 3, 1, "Etudiant dormeur"},            // type de base
    {'M', 9, 1, "Master"},                      // Gros dégâts mais très lent
    {'S', 2, 2, "Etudiant Talent sprinteur"},   // plus rapide mais moins résistant
    {'D', 8, 1, "Doctorant"},                   // Résistant et soigne les ennemis de 1 pv par tour de sa ligne.
    {'A', 4, 1, "Alternant"},                   // Change de ligne une fois dans la partie en sautant aléatoirement sur une des deux lignes adjacentes
};


// retourne le type d'ennemi correspondant au symbole
const TypeEnnemi* trouverTypeEnnemi(char symbole) {
    for(int i = 0; i < NB_TYPES_ENNEMIS; i++) {
        if(TYPES_ENNEMIS[i].symbole == symbole) {
            return &TYPES_ENNEMIS[i];
        }
    }
    return NULL;
}

// vérifie que le type est dans les types autorisés
const TypeEnnemi* VerifType(int *tour, int *ligne, char *symbole, Erreur *erreur) {
    const TypeEnnemi* type_ennemi = trouverTypeEnnemi(*symbole);
    if (type_ennemi == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "type d'ennemi invalide\n");
        return NULL;
    }
    return type_ennemi;
}

// on considère que chaque élement dans le fichier d'apparition d'un ennemi est unique et triée par ordre croissant
// tri d'abord par tour puis par ligne
Etudiant* InitialisationEnnemis(FILE* fichier_ennemis, Jeu* jeu, Erreur* erreur) {
    if (fichier_ennemis == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "le fichier n'a pas pu être ouvert\n");
        return NULL;
    }

    Etudiant* premier = NULL;  // premier ajouté
    Etudiant* dernier = NULL;  // dernier ajouté
    Etudiant* lignes_ennemis[NB_LIGNES] = {NULL}; // tableau des derniers ajoutés par ligne

    char ligne_fichier[256];

    while (fgets(ligne_fichier, sizeof(ligne_fichier), fichier_ennemis)) {
        int tour, num_ligne;
        char symbole_type;
        // pour chaque ligne on veut avoir trois valeurs corrcetes
        if (sscanf(ligne_fichier, "%d %d %c", &tour, &num_ligne, &symbole_type) != 3) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "format de ligne invalide\n");
            return NULL;
        }
        // obtenir type d'ennemi à partir du symbole correspondant
        const TypeEnnemi* type = VerifType(&tour, &num_ligne, &symbole_type, erreur);
        if (erreur->statut_erreur == 1) {
            LibererEnnemis(premier);
            return NULL;
        }

        // espace mémoire alloué pour l'étudiant
        Etudiant* nouvel_etudiant = (Etudiant*)malloc(sizeof(Etudiant));
        if (nouvel_etudiant == NULL) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "erreur d'allocation mémoire (ennemi)\n");
            LibererEnnemis(premier);
            return NULL;
        }

        // num_ligne commence à 1
        nouvel_etudiant->ligne = num_ligne;
        nouvel_etudiant->tour = tour;
        nouvel_etudiant->position = NB_EMPLACEMENTS + 99;

        nouvel_etudiant->next = NULL;
        nouvel_etudiant->next_line = NULL;
        nouvel_etudiant->prev_line = NULL;
        
        nouvel_etudiant->vitesse = type->vitesse;
        nouvel_etudiant->immobilisation = 0;
        nouvel_etudiant->type = (int)type->symbole;
        nouvel_etudiant->pointsDeVie = type->pointsDeVie;

        // ajout à la fin de la liste principale
        if (premier == NULL) {
            premier = nouvel_etudiant;
        } else {
            dernier->next = nouvel_etudiant;
        }
        dernier = nouvel_etudiant;

        // chaînage par ligne
        if (lignes_ennemis[num_ligne - 1] == NULL) {
            lignes_ennemis[num_ligne - 1] = nouvel_etudiant;
        } else {
            nouvel_etudiant->prev_line = lignes_ennemis[num_ligne - 1];
            lignes_ennemis[num_ligne - 1]->next_line = nouvel_etudiant;
            // on met à jour la référence au dernier ennemi de la ligne de jeu
            lignes_ennemis[num_ligne - 1] = nouvel_etudiant;
        }
    }

    return premier;
}



// libère dynamiquement la mémoire allouée pour les ennemis
void LibererEnnemis(Etudiant* premier) {
    while (premier != NULL) {
        Etudiant* courant = premier;
        premier = premier->next;
        free(courant);
    }
}

void SupprimerEnnemi(Jeu* jeu, Erreur* erreur, Etudiant* ennemi) {
    if (!ennemi || !jeu) {
        erreur->statut_erreur=1;
        strcpy(erreur->msg_erreur, "Erreur d'accès à l'ennemi ou au jeu\n");
    }

    AjouterAuScore(jeu, ennemi, erreur);

    // chainage
    if (ennemi == jeu->etudiants) {
        // premier de la liste
        jeu->etudiants = ennemi->next;
    } else {
        // on trouve l'ennemi précédent
        Etudiant* prec = jeu->etudiants;
        while (prec && prec->next != ennemi) {
            prec = prec->next;
        }
        if (!prec) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Impossible de supprimer l'ennemi : ennemi introuvable\n");
            return;
        }
        prec->next = ennemi->next;
    }

    // chaînage par ligne
    if (ennemi->prev_line) {
        ennemi->prev_line->next_line = ennemi->next_line;
    }
    if (ennemi->next_line) {
        ennemi->next_line->prev_line = ennemi->prev_line;
    }
    free(ennemi);
}