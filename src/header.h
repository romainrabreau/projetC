#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// ---------------------------------------------------------------------
// Définitions de constantes
// ---------------------------------------------------------------------
#define NB_LIGNES         7
#define NB_EMPLACEMENTS   15
#define NB_TYPES_ENNEMIS  3
#define NB_ENNEMIS_MAX    1000
#define NB_TYPES_TOURELLES 1

// ---------------------------------------------------------------------
// Structures
// ---------------------------------------------------------------------
typedef struct {
    char msg_erreur[100];
    int statut_erreur;
} Erreur;

/*
La structure Etudiant modélise les ennemis :
  - type correspond à la valeur ASCII du symbole associé (ex. 'Z', 'T', etc.)
  - pointsDeVie
  - ligne, position, vitesse
  - tour d'apparition
  - chaînage principal (next) et chaînage par ligne (next_line, prev_line).
*/
typedef struct etudiant {
    int type;
    int pointsDeVie;
    int ligne;
    int position;
    int vitesse;
    int tour;
    struct etudiant* next;       // chaînage global
    struct etudiant* next_line;  // chaînage par ligne
    struct etudiant* prev_line;  // chaînage par ligne
} Etudiant;

typedef struct tourelle {
    int type;
    int pointsDeVie;
    int ligne;
    int position;
    int prix;
    struct tourelle* next;
} Tourelle;

// Types disponibles pour les ennemis
typedef struct {
    const char symbole;     // Caractère représentant le type dans le fichier
    int pointsDeVie;
    int degats;
    int vitesse;
    const char* nom;        // Nom du type
} TypeEnnemi;

// Types disponibles pour les tourelles
typedef struct {
    const char symbole;
    int pointsDeVie;
    int degats;
    int prix;
    const char* nom;
} TypeTourelle;

// Structure de jeu globale
typedef struct {
    Tourelle* tourelles;   // Liste chaînée de tourelles
    Etudiant* etudiants;   // Liste chaînée de tous les ennemis
    int cagnotte;
    int tour;
} Jeu;

// ---------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------

// 1) Fonctions d'introduction / affichage
void IntroduireJeu(Erreur* erreur);
void VisualiserEnnemis(Etudiant* etudiants, Erreur* erreur);

// 2) Gestion des ennemis
Etudiant* InitialiserEnnemis(FILE* fichier_ennemis, Jeu* jeu, Erreur* erreur);
void LibererEnnemis(Etudiant* premier);

// 2b) Fonctions internes (si vous les appelez vraiment)
const TypeEnnemi* trouverTypeEnnemi(char symbole); 
// (ou bien "trouverType" — à unifier avec votre code)


// 3) Gestion des tourelles
Tourelle* InitialiserTourelles(int* cagnotte, Erreur* erreur);
int VerifEntreeLigne(char* ligne_tourelles);
Tourelle* AjouterTourelles(Tourelle* premier, char* ligne_tourelles, int ligne);
void LibererTourelles(Tourelle* premier);

const TypeTourelle* trouverTypeTourelle(char symbole);

// 4) Fonctions d'aide
void ResoudreFichier(FILE* fichier_ennemis, Erreur* erreur);

// 5) Fonctions liées au déroulement de la partie
void JouerPartie(Jeu* jeu, Erreur* erreur);
void AfficherPlateau(Jeu* jeu);

void supprimerTourelle(Jeu* jeu, Tourelle* t);
void supprimerEtudiant(Jeu* jeu, Etudiant* e);


// (Si besoin) prototypes supplémentaires
// ex. si vous avez vraiment une fonction VerifType(...) 
// ou LibererMemEnnemis(...), il faut aussi les déclarer ici.
//
// Ex. : 
//   TypeEnnemi* VerifType(int *tour, int *ligne, char *type, Erreur *erreur);
//   void LibererMemEnnemis(Etudiant* premier);

#endif