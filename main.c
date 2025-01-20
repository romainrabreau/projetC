#include "header.h"

int main(int argc, char *argv[]) {
    if (argc!=2) {
        printf("assurez-vous d'avoir entré exactement un nom de fichier\n");
        return 1;
    }
    char *test = strstr(argv[1], ".txt");
    if (strstr(test, ".txt")==NULL) {
        printf("assurez-vous d'avoir entré un nom de fichier se terminant par .txt\n");
        return 1;
    }
    
    FILE * fichier_ennemis = fopen(argv[1], "r");
    if (fichier_ennemis == NULL) {
        printf("le fichier n'a pas pu être ouvert\n");
        return 1;
    }
    // structure pour conserver le statut et le message d'erreur
    Erreur erreur;
    Jeu jeu;
    InitialiserJeu(&erreur, &jeu, fichier_ennemis);
    if (erreur.statut_erreur==1) {
        printf("%s", erreur.msg_erreur);
        return 1;
    }
    return 0;
}

// Initialiser structure de données du jeu des tourelles et des ennemis
void InitialiserJeu(Erreur *erreur, Jeu *jeu, FILE *fichier_ennemis){
    if (erreur->statut_erreur==1) {
        return;
    }
    if (fichier_ennemis == NULL) {
        erreur->statut_erreur=1;
        strcpy(erreur->msg_erreur, "le fichier n'a pas pu être ouvert\n");
        return;
    }
    if (jeu == NULL) {
        erreur->statut_erreur=1;
        strcpy(erreur->msg_erreur, "le jeu n'a pas pu être initialisé\n");
        return;
    }
    int cagnotte=0;
    char newline;
    // vérifier que la première ligne contient uniquement le solde et le changement de ligne
    if (fscanf(fichier_ennemis, "%d%c", &cagnotte, &newline) != 2 || newline != '\n') {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "format de la cagnotte invalide\n");
        return;
    }
    jeu->cagnotte=cagnotte;
    // on trie le fichier avec les ennemis et on résouts les doublons
    ResoudreFichier(fichier_ennemis, erreur);
    if (erreur->statut_erreur==1) {
        return;
    }
    IntroduireJeu(erreur);
    Etudiant etudiants[] = InitialisationEnnemis(fichier_ennemis, jeu, &erreur);
    if (erreur->statut_erreur==1) {
        return;
    }
    // pointe vers le premier ennemi
    jeu->etudiants=etudiants;
    //TODO
    //VisualiserEnnemis(jeu->etudiants, erreur);
    //TODO
    Tourelle tourelles[] = InitialisationTourelles(&jeu->cagnotte, &erreur);
    if (erreur->statut_erreur==1) {
        return;
    }
    // pointe vers la première tourelle
    jeu->tourelles = tourelles;


    return;
}

void LibererJeu(Jeu* jeu) {
    LibererEnnemis(jeu->etudiants);
    LibererTourelles(jeu->tourelles);
}

void JouerTour(Jeu* jeu, Erreur* erreur) {
    while (18) {
        jeu->tour++;

        printf("Tour %d\n", jeu->tour);

        // Étape 1 : Entrée des ennemis au tour courant
        ApparitionEnnemis(jeu, erreur);
        if (erreur->statut_erreur == 1) return;

        // Étape 2 : Résolution des actions des tourelles
        ResoudreActionsTourelles(jeu, erreur);
        if (erreur->statut_erreur == 1) return;

        // Étape 3 : Résolution des actions des ennemis (combat avec tourelles)
        ResoudreActionsEnnemis(jeu, erreur);
        if (erreur->statut_erreur == 1) return;

        // Étape 4 : Déplacement des ennemis
        DeplacerEnnemis(jeu, erreur);
        if (erreur->statut_erreur == 1) return;

        // Étape 5 : Vérification des conditions de fin de jeu
        if (VerifierDefaite(jeu)) {
            printf("Vous avez perdu ... Les ennemis ont pris le contrôle de l'université.\n");
            break;
        }

        if (VerifierVictoire(jeu)) {
            printf("Victoire, bien joué. Vous avez défendu l'université contre tous les étudiants.\n");
            break;
        }
        // nettoyer le \n restant
        while ((getchar()) != '\n');
        // Pause pour simuler le déroulement du jeu
        printf("Appuyez sur Entrée pour continuer...\n");
        getchar();
    }
}
