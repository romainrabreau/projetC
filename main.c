#include "header.h"



// Initialiser structures de données du jeu des tourelles et des ennemis
void InitialiserJeu(Erreur *erreur, Jeu *jeu, FILE *fichier_ennemis){
    if (erreur->statut_erreur==1) {
        return;
    }

    //IntroduireJeu(erreur);
    if (erreur->statut_erreur==1) {
        return;
    }
    printf("     𓀡   Initialisation du jeu en cours ...     \n\n");
    //barre_de_chargement(2000);
    // animer_attente(2000, " Initialisation du jeu en cours ...");
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
    jeu->tour=0;
    printf("\nVous disposez blabla\n\n");
    printf("Cagnotte : %d\n\n", jeu->cagnotte);
    // on trie le fichier avec les ennemis et on résouts les doublons
    //ResoudreFichier(fichier_ennemis, erreur);
    if (erreur->statut_erreur==1) {
        return;
    }

    printf(ANSI_BG_GRIS_CLAIR ANSI_TEXTE_BLEU_FONCE "  ❃  Initialisation des ennemis en cours ..."ANSI_RESET"\n\n");

    Etudiant * etudiants = InitialisationEnnemis(fichier_ennemis, jeu, erreur);
    if (erreur->statut_erreur==1) {
        return;
    }
    // pointe vers le premier ennemi
    jeu->etudiants=etudiants;

    printf("◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆\n");


    printf(ANSI_TEXTE_GRIS "Appuyez sur Entrée pour continuer...\n" ANSI_RESET);
    while ((getchar()) != '\n');

    VisualiserEnnemis(jeu->etudiants, erreur);
    if (erreur->statut_erreur==1) {
        return;
    }

    printf(ANSI_TEXTE_GRIS"Appuyez sur Entrée pour continuer...\n"ANSI_RESET);
    while ((getchar()) != '\n');
   
    printf(ANSI_BG_GRIS_CLAIR ANSI_TEXTE_BLEU_FONCE "  ❃  Initialisation des tourelles en cours ..."ANSI_RESET"\n\n");

    Tourelle * tourelles = InitialisationTourelles(&jeu->cagnotte, erreur);
    if (erreur->statut_erreur==1) {
        return;
    }
    // pointe vers la première tourelle
    jeu->tourelles = tourelles;
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
    char len = strlen(argv[1]);
    if (len < 4 || strcmp(argv[1] + len - 4, ".txt") != 0) {
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

    // initialisation de la graine pour les fonctions aléatoires
    srand((unsigned int)(uintptr_t)&erreur);
    InitialiserJeu(&erreur, &jeu, fichier_ennemis);
    if (erreur.statut_erreur==1) {
        fclose(fichier_ennemis);
        printf(ANSI_BG_ROUGE ANSI_TEXTE_BLANC "%s" ANSI_RESET, erreur.msg_erreur);
        LibererJeu(&jeu);
        return 1;
    }
    fclose(fichier_ennemis);

    JouerPartie(&jeu, &erreur);

    if (erreur.statut_erreur == 1) {
        printf(ANSI_BG_ROUGE ANSI_TEXTE_BLANC "%s" ANSI_RESET, erreur.msg_erreur);
    }

    LibererJeu(&jeu);
    return 0;
}