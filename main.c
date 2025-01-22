#include "src/header.h"

// Initialiser structure de données du jeu
void InitialiserJeu(Erreur *erreur, Jeu *jeu, FILE *fichier_ennemis) {
    // Vérifications
    if (erreur->statut_erreur == 1) {
        return;
    }
    if (!fichier_ennemis) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Le fichier n'a pas pu être ouvert\n");
        return;
    }
    if (!jeu) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Le jeu n'a pas pu être initialisé\n");
        return;
    }

    int cagnotte = 0;
    char newline;
    // Vérifier que la première ligne contient uniquement la cagnotte et un '\n'
    if (fscanf(fichier_ennemis, "%d%c", &cagnotte, &newline) != 2 || newline != '\n') {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format de la cagnotte invalide\n");
        return;
    }
    // On stocke la cagnotte dans la structure de jeu
    jeu->cagnotte = cagnotte;
    /*
    // On trie le fichier et résout d'éventuels doublons (selon votre logique)
    ResoudreFichier(fichier_ennemis, erreur);
    if (erreur->statut_erreur == 1) {
        return;
    }
    */
    // Présentation du jeu (affichage d'intro, etc.)
    IntroduireJeu(erreur);
    if (erreur->statut_erreur == 1) {
        return;
    }

    // Initialisation des ennemis
    Etudiant *etudiants = InitialiserEnnemis(fichier_ennemis, jeu, erreur);
    if (erreur->statut_erreur == 1) {
        return;
    }
    jeu->etudiants = etudiants;

    // Affichage initial des ennemis (optionnel)
    VisualiserEnnemis(jeu->etudiants, erreur);
    if (erreur->statut_erreur == 1) {
        return;
    }

    // Initialisation des tourelles
    Tourelle *tourelles = InitialiserTourelles(&jeu->cagnotte, erreur);
    if (erreur->statut_erreur == 1) {
        return;
    }
    jeu->tourelles = tourelles;
}

// Libère la mémoire (ennemis + tourelles) avant de quitter
void LibererJeu(Jeu* jeu) {
    if (!jeu) return;

    LibererEnnemis(jeu->etudiants);
    LibererTourelles(jeu->tourelles);
}

int main(int argc, char *argv[]) {
    printf("\033[0;0H");
    printf("\033[2J");
    if (argc != 2) {
        printf("Assurez-vous d'avoir entré exactement un nom de fichier\n");
        return 1;
    }

    // Vérifie que le nom de fichier se termine par ".txt"
    char *pos_ext = strstr(argv[1], ".txt");
    if (!pos_ext || strcmp(pos_ext, ".txt") != 0) {
        printf("Assurez-vous d'avoir entré un nom de fichier se terminant par .txt\n");
        return 1;
    }

    FILE *fichier_ennemis = fopen(argv[1], "r+");
    if (!fichier_ennemis) {
        printf("Le fichier n'a pas pu être ouvert\n");
        return 1;
    }

    Erreur erreur;
    erreur.statut_erreur = 0;
    strcpy(erreur.msg_erreur, "");

    Jeu jeu;
    memset(&jeu, 0, sizeof(Jeu));

    InitialiserJeu(&erreur, &jeu, fichier_ennemis);
    if (erreur.statut_erreur == 1) {
        printf("%s", erreur.msg_erreur);
        fclose(fichier_ennemis);
        return 1;
    }

    fclose(fichier_ennemis);

    JouerPartie(&jeu, &erreur);
    if (erreur.statut_erreur == 1) {
        printf("%s", erreur.msg_erreur);
    }

    LibererJeu(&jeu);

    return 0;
}
