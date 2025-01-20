#ifndef HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define HEADER_H
#define NB_LIGNES 7
#define NB_EMPLACEMENTS 15
#define NB_TYPES_ENNEMIS 3
#define NB_ENNEMIS_MAX 1000
#define NB_TYPES_TOURELLES 1

/// definition des structures du jeu
typedef struct {
    char msg_erreur[100];
    int statut_erreur;
} Erreur;

/*
La structure Etudiant modélise les vilains ennemis. Ces derniers ont un type, un certain
nombre de points de vie, sont placés sur une ligne et à une position données. Ils ont une vitesse
de déplacement et apparaissent à un certain tour. Les vilains ennemis sont chaînés par ordre
d’apparition dans le jeu via le champ next mais ils sont de plus doublement chaînés par ligne
(champs next_line et prev_line). On peut ainsi rapidement connaître celui qui précède et celui qui suit sur la même ligne. Ces trois pointeurs devront toujours être à jour durant l’exécution
du programme, à chaque insertion mais aussi à chaque suppression.
*/
typedef struct etudiant {
    int type;
    int pointsDeVie;
    int ligne;
    int position;
    int vitesse;
    int tour;
    struct etudiant* next;
    struct etudiant* next_line;
    struct etudiant* prev_line;
} Etudiant;

typedef struct tourelle {
    int type;
    int pointsDeVie;
    int ligne;
    int position;
    int prix;
    struct tourelle* next;
} Tourelle;

typedef struct {
    const char symbole; // Caractère représentant le type dans le fichier
    int pointsDeVie;
    int vitesse;
    const char* nom;     // Nom du type
} TypeEnnemi;

typedef struct {
    const char symbole;        
    int pointsDeVie;   
    int prix;
    const char* nom;    // Nom du type
} TypeTourelle;

typedef struct {
    Tourelle* tourelles;
    // pointeur vers le premier ennemi
    Etudiant* etudiants;
    int cagnotte;
    int tour;
} Jeu;


// prototypes de fonctions de gestion des ennemis
const TypeEnnemi* trouverType(char symbole);
void InitialiserEnnemiType(Etudiant* etudiant, const TypeEnnemi* type);
Etudiant* InitialisationEnnemis(FILE* fichier_ennemis, Jeu* jeu, Erreur* erreur);

// protopypes de fonctions de gestion des tourelles
Tourelle * InitialiserTourelles(int * cagnotte, Erreur* erreur);
int VerifEntreeLigne(char * ligne_tourelles);
void LibererTourelles(Tourelle* premier);
void AjouterTourelles(Tourelle* premier, char* ligne_tourelles, int ligne);

// fonctions d'aide
void ResoudreFichier(FILE* fichier_ennemis, Erreur* erreur);

#endif