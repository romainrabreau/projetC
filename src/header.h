#ifndef HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>


#define HEADER_H
#define NB_LIGNES 7
#define NB_EMPLACEMENTS 15
#define NB_TYPES_ENNEMIS 6
#define NB_TYPES_TOURELLES 6
#define MAX_SCORES 10
#define MAX_NIVEAUX 10
#define MAX_NAME_LEN 256
#define MAX_SAVE 100


// couleurs de texte de sortie terminal

#define ANSI_RESET "\033[0m"
#define CLEAR_LINE "\033[2K"

#define ANSI_TEXTE_BLANC "\033[38;5;15m"
#define ANSI_TEXTE_BLEU_FONCE "\033[38;5;12m"
#define ANSI_TEXTE_BLEU_MOYEN "\033[38;5;25m"
#define ANSI_TEXTE_GRIS "\033[38;5;252m"

#define ANSI_BG_BLEU_SHINY "\033[104m" // bleu éléctrique
#define ANSI_BG_BLANC "\033[48;5;15m"
#define ANSI_BG_BLEU_MEGA_LIGHT "\033[48;5;195m" // bleu ECTS
#define ANSI_BG_BLEU_CLAIR "\033[48;5;81m" // Steel blue vrmt clair
#define ANSI_BG_GRIS_CLAIR "\033[48;5;254m"
#define ANSI_BG_VERT_FONCE "\033[48;5;37m"
#define ANSI_BG_BLEU_FONCE "\033[48;5;17m" // Navy blue
#define ANSI_BG_ROUGE "\033[48;5;124m"


extern char pseudo[];

/// definition des structures du jeu
typedef struct {
    char msg_erreur[150];
    int statut_erreur;
} Erreur;


typedef struct etudiant {
    // type correspond a la valeur ASCII du symbole associé
    int type;
    int pointsDeVie;
    int ligne;
    int position;
    int vitesse;
    int tour;
    int immobilisation;
    int deplace;// 1 si l'ennemi a été déplacé, uniquement pour le type F
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
    const char* nom;     
    const char* description; 
} TypeEnnemi;

typedef struct {
    const char symbole;        
    int pointsDeVie;   
    int prix;
    const char* nom;
    const char* description;
} TypeTourelle;

extern const TypeEnnemi TYPES_ENNEMIS[];
extern const TypeTourelle TYPES_TOURELLES[];

typedef struct {
    // pointeur vers la première tourelle
    Tourelle* tourelles;
    // pointeur vers le premier ennemi
    Etudiant* etudiants;
    int cagnotte;
    int tour;
    int score;
    char pseudo[50];
    char fichier_ennemis[255];
} Jeu;

// prototypes de fonctions du main
void PreparerPartie(Erreur* erreur, Jeu* jeu, const char* chemin_niveau);

//prototypes de fonctions d'introduction 
void IntroduireJeu();
char* Menu(Erreur* erreur);

// prototypes de fonctions de gestion des ennemis
const TypeEnnemi* trouverTypeEnnemi(char symbole);
const TypeEnnemi* VerifType(int *tour, int *ligne, char *symbole, Erreur *erreur);
Etudiant* InitialisationEnnemis(FILE* fichier_ennemis, Jeu* jeu, Erreur* erreur);
void SupprimerEnnemi(Jeu* jeu, Erreur* erreur, Etudiant* ennemi);
void LibererEnnemis(Etudiant* premier);
int ActionFaineant(Jeu* jeu, Etudiant* e);

// protopypes de fonctions de gestion des tourelles
Tourelle * InitialisationTourelles(int * cagnotte, Erreur* erreur);
int VerifEntreeLigne(char * ligne_tourelles, Erreur* erreur);
void LibererTourelles(Tourelle* premier);
void SupprimerTourelle(Jeu* jeu, Erreur* erreur, Tourelle* tourelle);
Tourelle* AjouterTourelles(Tourelle* * premier, Tourelle* dernier, char* ligne_tourelles, int ligne, Erreur* erreur);

// fonctions d'aide
void ResoudreFichier(FILE* fichier_ennemis, Erreur* erreur);
void Attendre(int ms);
char (*LectureNoms(char *dossierChemin, int *nb, Erreur *err))[MAX_NAME_LEN];
char (*FormatterNoms(char noms[][MAX_NAME_LEN], int nb, Erreur *err))[MAX_NAME_LEN];
void ChoixLeaderboard(Erreur *err);
char* RecupererNom(const char* chemin, Erreur* erreur);

// prototypes de fonctions de jeu
void JouerPartie(Jeu* jeu, Erreur* erreur);
void AfficherPlateau(Jeu* jeu);

// prototypes de visualisation
void AnimerAttente(int attente_ms, char *message);
void printAvecDelai(const char *text, int delai_ms);
void BarreChargement(int ms);
void AfficherTitre();
void IntroMenu();
void VisualiserEnnemis(Etudiant* etudiants);
int AfficherChoix(char options[][MAX_NAME_LEN], int n_options, Erreur *err);

// prototypes de fonctions de score
void AjouterAuScore(Jeu* jeu, Etudiant* e, Erreur* erreur);
void AjouterAuLeaderboard(Jeu* jeu, Erreur* erreur);
void AfficherLeaderboard(const char *nomLeaderboard, Erreur *err);

// prototypes de fonctions de sauvegarde de partie
void SauvegarderPartie(Jeu* jeu, Erreur* erreur);
void RelancerPartie(Erreur* erreur, Jeu* jeu, const char* chemin_save);

#endif