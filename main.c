#include "src/header.h"

// Initialiser structures de données du jeu des tourelles et des ennemis
void InitialiserJeu(Erreur *erreur, Jeu *jeu, FILE *fichier_ennemis){
    if (erreur->statut_erreur==1) {
        return;
    }

    IntroduireJeu(erreur);
    if (erreur->statut_erreur==1) {
        return;
    }
    animer_attente(2000, "Initialisation du jeu en cours ...");
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
    //ResoudreFichier(fichier_ennemis, erreur);
    if (erreur->statut_erreur==1) {
        return;
    }

    // Initialisation du peudo du joueur.
    printf("Entrez votre pseudo : ");
    scanf("%49s", jeu->pseudo);
    while (getchar() != '\n');
    animer_attente(1000, "Ajout du pseudo...");


    printf("\033[36;47mInitialisation des ennemis en cours ..."RESET"\n");

    Etudiant * etudiants = InitialisationEnnemis(fichier_ennemis, jeu, erreur);
    if (erreur->statut_erreur==1) {
        return;
    }
    // pointe vers le premier ennemi
    jeu->etudiants=etudiants;

    VisualiserEnnemis(jeu->etudiants, erreur);
    if (erreur->statut_erreur==1) {
        return;
    }
    printf("\033[36;47mInitialisation des tourelles en cours ..."RESET"\n");

    // Visualiser tourelles.
    Tourelle * tourelles = InitialisationTourelles(&jeu->cagnotte, erreur);
    if (erreur->statut_erreur==1) {
        return;
    }
    // pointe vers la première tourelle
    jeu->tourelles = tourelles;
    jeu->score = 0;
    return;
}

void LibererJeu(Jeu* jeu) {
    if (jeu == NULL) {
        return;
    }
    LibererEnnemis(jeu->etudiants);
    LibererTourelles(jeu->tourelles);
}

int main(int argc, char *argv[]) {
    printf("\033[0;0H");
    printf("\033[2J");
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
    erreur.statut_erreur = 0;
    Jeu jeu;

    strncpy(jeu.fichier_ennemis, argv[1], sizeof(jeu.fichier_ennemis) - 1);
    jeu.fichier_ennemis[sizeof(jeu.fichier_ennemis) - 1] = '\0';


    InitialiserJeu(&erreur, &jeu, fichier_ennemis);
    if (erreur.statut_erreur==1) {
        fclose(fichier_ennemis);
        printf("%s", erreur.msg_erreur);
        LibererJeu(&jeu);
        return 1;
    }
    fclose(fichier_ennemis);

    JouerPartie(&jeu, &erreur);
    AfficherLeaderboard();

    if (erreur.statut_erreur == 1) {
        printf("%s", erreur.msg_erreur);
    }

    LibererJeu(&jeu);
    return 0;
}