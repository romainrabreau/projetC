#ifndef HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>


#define HEADER_H
#define NB_LIGNES 7
#define NB_EMPLACEMENTS 15
#define NB_TYPES_ENNEMIS 5
#define NB_ENNEMIS_MAX 1000
#define NB_TYPES_TOURELLES 5

// couleurs de texte de sortie terminal
#define RESET "\033[0m"    
#define ROUGE "\033[1;31m"
#define VERT "\033[1;32m"
#define JAUNE "\033[1;33m"
#define BLEU "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m" 
#define BLANC "\033[1;37m"
#define CLEAR_LINE "\033[K"


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
    // type correspond a la valeur ASCII du symbole associé
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

//prototypes de fonctions d'introduction 
void IntroduireJeu(Erreur* erreur);
void VisualiserEnnemis(Etudiant* etudiants, Erreur* erreur);

// prototypes de fonctions de gestion des ennemis
const TypeEnnemi* trouverType(char symbole);
const TypeEnnemi* VerifType(int *tour, int *ligne, char *symbole, Erreur *erreur);
Etudiant* InitialisationEnnemis(FILE* fichier_ennemis, Jeu* jeu, Erreur* erreur);
void SupprimerEnnemi(Jeu* jeu, Erreur* erreur, Etudiant* ennemi);
void LibererEnnemis(Etudiant* premier);

// protopypes de fonctions de gestion des tourelles
Tourelle * InitialisationTourelles(int * cagnotte, Erreur* erreur);
int VerifEntreeLigne(char * ligne_tourelles, Erreur* erreur);
void LibererTourelles(Tourelle* premier);
Tourelle* AjouterTourelles(Tourelle* premier, Tourelle* dernier, char* ligne_tourelles, int ligne, Erreur* erreur);
// fonctions d'aide
void ResoudreFichier(FILE* fichier_ennemis, Erreur* erreur);
void Attendre(int ms);

// prototypes de fonctions de jeu
void JouerPartie(Jeu* jeu, Erreur* erreur);
void AfficherPlateau(Jeu* jeu);

// prototypes de visualisation
void animer_attente(int attente_ms, char *message);
void print_avec_delai(const char *text, int delai_ms);

#endif