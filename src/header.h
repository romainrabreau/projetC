#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

/* Définition de constantes */
#define NB_LIGNES         7
#define NB_EMPLACEMENTS   15
#define NB_TYPES_ENNEMIS   5
#define NB_ENNEMIS_MAX   1000
#define NB_TYPES_TOURELLES 6
#define MAX_SCORES         10
#define MAX_NIVEAUX       100
#define MAX_SAVE          100
#define MAX_NAME_LEN     256

/* Variables globales */
extern char pseudo[];

/* Couleurs pour affichage terminal */
#define ANSI_WHITE_TEXT    "\033[97m"
#define ANSI_BLUE_TEXT     "\033[94m"
#define ANSI_RESET         "\033[0m"
#define ANSI_BLUE_BG       "\033[44m"
#define ANSI_LIGHT_BLUE_BG "\033[104m"

#define RESET    "\033[0m"    
#define ROUGE    "\033[1;31m"
#define VERT     "\033[1;32m"
#define JAUNE    "\033[1;33m"
#define BLEU     "\033[1;34m"
#define MAGENTA  "\033[1;35m"
#define CYAN     "\033[1;36m" 
#define BLANC    "\033[1;37m"
#define CLEAR_LINE "\033[K"

/* Définition de la structure d'erreur */
typedef struct {
    char msg_erreur[100];
    int statut_erreur;
} Erreur;

/* Structures du jeu */

/* Structure Etudiant :
   Modélise un ennemi (étudiant) avec ses caractéristiques, ainsi que
   des pointeurs pour le chaînage global et par ligne.
*/
typedef struct etudiant {
    int type;             // Valeur ASCII du symbole associé
    int pointsDeVie;
    int ligne;
    int position;
    int vitesse;
    int tour;
    int immobilisation;
    struct etudiant* next;
    struct etudiant* next_line;
    struct etudiant* prev_line;
} Etudiant;

/* Structure Tourelle : */
typedef struct tourelle {
    int type;
    int pointsDeVie;
    int ligne;
    int position;
    int prix;
    struct tourelle* next;
} Tourelle;

/* Type d'ennemi et de tourelle */
typedef struct {
    const char symbole; // Symbole représentant le type
    int pointsDeVie;
    int vitesse;
    const char* nom;    // Nom du type
} TypeEnnemi;

typedef struct {
    const char symbole;
    int pointsDeVie;
    int prix;
    const char* nom;    // Nom du type
} TypeTourelle;

/* Structure principale du jeu */
typedef struct {
    Tourelle* tourelles;
    Etudiant* etudiants;  // Pointeur vers le premier ennemi
    int cagnotte;
    int score;
    char pseudo[50];
    char fichier_ennemis[256];
    int tour;
} Jeu;

/* Prototypes des fonctions du main */
char* Menu(Erreur *err);
void PreparerPartie(Erreur* erreur, Jeu* jeu, char* chemin_niveau);

/* Prototypes de fonctions de gestion des ennemis */
const TypeEnnemi* trouverType(char symbole);
const TypeEnnemi* VerifType(int *tour, int *ligne, char *symbole, Erreur *erreur);
Etudiant* InitialisationEnnemis(FILE* fichier_ennemis, Jeu* jeu, Erreur* erreur);
void SupprimerEnnemi(Jeu* jeu, Erreur* erreur, Etudiant* ennemi);
void LibererEnnemis(Etudiant* premier);

/* Prototypes de fonctions de gestion des tourelles */
Tourelle* InitialisationTourelles(int* cagnotte, Erreur* erreur);
int VerifEntreeLigne(char* ligne_tourelles, Erreur* erreur);
void LibererTourelles(Tourelle* premier);
Tourelle* AjouterTourelles(Tourelle* premier, Tourelle* dernier, char* ligne_tourelles, int ligne, Erreur* erreur);

/* Fonctions d'aide */
void ResoudreFichier(FILE* fichier_ennemis, Erreur* erreur);
void Attendre(int ms);
char (*LectureNoms(char *dossierChemin, int *nb, Erreur *err))[MAX_NAME_LEN];
char (*FormatterNoms(char noms[][MAX_NAME_LEN], int nb, Erreur *err))[MAX_NAME_LEN];
void ChoixLeaderboard(Erreur *err);
char* RecupererNom(const char* chemin);

/* Fonctions de scoring */
void AjouterAuScore(Jeu* jeu, Etudiant* e, Erreur* erreur);
void AjouterAuLeaderboard(Jeu* jeu, Erreur* erreur);
void AfficherLeaderboard(const char *nomLeaderboard, Erreur *err);

/* Prototypes de fonctions de jeu */
void JouerPartie(Jeu* jeu, Erreur* erreur);
void AfficherPlateau(Jeu* jeu);

/* Fonctions d'effets visuels et d'affichage */
void animer_attente(int attente_ms, char *message);
void print_avec_delai(const char *text, int delai_ms);
void AfficherTitre();
void IntroMenu();
void VisualiserEnnemis(Etudiant* etudiants, Erreur* erreur);
int AfficherChoix(char options[][MAX_NAME_LEN], int n_options, Erreur *err);

/* Fonctions de sauvegarde */
void SauvegarderPartie(Jeu* jeu, Erreur* erreur);
void RelancerPartie(Erreur* erreur, Jeu* jeu, const char* chemin_save);

/* Accès aux types d'ennemis et de tourelles */
extern const TypeEnnemi TYPES_ENNEMIS[];
extern const TypeTourelle TYPES_TOURELLES[];

#endif 
