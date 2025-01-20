#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Ajustez le chemin si besoin, par exemple "../src/header.h"
#include "../src/header.h"  

// Prototypes (si header.h ne les déclare pas déjà)
void VisualiserEnnemis(Etudiant* etudiants, Erreur* erreur);

// Fonction pour libérer la liste chaînée plus tard
static void LibererListeEnnemis(Etudiant* premier) {
    while (premier != NULL) {
        Etudiant* temp = premier;
        premier = premier->next;
        free(temp);
    }
}

int main(void) {
    // 1) Ouvrir le fichier vilains1.txt
    FILE* f = fopen("vilains1.txt", "r");
    if (!f) {
        printf("Impossible d’ouvrir le fichier vilains1.txt\n");
        return 1;
    }

    // 2) Lire la 1re ligne (la cagnotte) et l’ignorer
    //    On suppose que c’est un entier + un '\n'
    int cagnotte;
    if (fscanf(f, "%d", &cagnotte) != 1) {
        printf("Erreur de lecture de la cagnotte dans vilains1.txt\n");
        fclose(f);
        return 1;
    }
    // Lire le reste de la ligne pour s’assurer de passer au suivant
    while (!feof(f) && fgetc(f) != '\n') {
        ; // on vide la fin de la ligne
    }

    // 3) Parcourir le fichier pour construire la liste chaînée
    Etudiant* premier = NULL;    // pointeur vers le 1er ennemi
    Etudiant* dernier = NULL;    // pointeur vers le dernier ennemi

    int vague, ligne;
    char type_char;

    // On lit jusqu’à la fin du fichier
    while (fscanf(f, "%d %d %c", &vague, &ligne, &type_char) == 3) {
        // Créer un nouvel ennemi
        Etudiant* nouveau = (Etudiant*)malloc(sizeof(Etudiant));
        if (!nouveau) {
            printf("Erreur d’allocation mémoire\n");
            // Libère ce qu’on a déjà
            LibererListeEnnemis(premier);
            fclose(f);
            return 1;
        }
        // Initialisation minimale
        nouveau->type        = type_char;  // ex: 'Z'
        nouveau->pointsDeVie = 5;          // à ajuster selon vos règles
        nouveau->ligne       = ligne;      // 1..7 si c’est votre convention
        nouveau->tour        = vague;      // champ “vague” = “tour” dans votre code
        nouveau->position    = 0;          // position “0” par défaut, ou -1
        nouveau->vitesse     = 1;          // ex. par défaut
        nouveau->next        = NULL;
        nouveau->next_line   = NULL;
        nouveau->prev_line   = NULL;

        // Chaînage
        if (!premier) {
            // premier ennemi de la liste
            premier = nouveau;
            dernier = nouveau;
        } else {
            dernier->next = nouveau;
            dernier = nouveau;
        }
    }

    fclose(f);

    // 4) On appelle VisualiserEnnemis(...) pour afficher
    Erreur erreur;
    erreur.statut_erreur = 0;
    strcpy(erreur.msg_erreur, "");

    VisualiserEnnemis(premier, &erreur);

    if (erreur.statut_erreur) {
        printf("Erreur : %s\n", erreur.msg_erreur);
    } else {
        printf("Visualisation des vagues terminée.\n");
    }

    // 5) Libération mémoire
    LibererListeEnnemis(premier);

    return 0;
}
