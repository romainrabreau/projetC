#include "header.h"

char pseudo[50];

/*
 Prépare la partie à partir du fichier de niveau indiqué par chemin_niveau.
 1) Ouvre le fichier du niveau et lit la cagnotte.
 2) Initialise les ennemis via InitialisationEnnemis.
 3) Affiche les ennemis via VisualiserEnnemis.
 4) Initialise les tourelles via InitialisationTourelles.
 Le chemin du fichier est également sauvegardé dans la structure de jeu.
 */
void PreparerPartie(Erreur* erreur, Jeu* jeu, const char* chemin_niveau) {
    if (erreur->statut_erreur) return;
    if (jeu == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Structure de jeu non initialisée");
        return;
    }

    // on lit le fichier de niveau fourni
    FILE* fichier = fopen(chemin_niveau, "r");
    if (!fichier) {
        erreur->statut_erreur = 1;
        snprintf(erreur->msg_erreur, sizeof(erreur->msg_erreur), "Impossible d'ouvrire le fichier : %s", chemin_niveau);
        return;
    }

    // Stockage du chemin du fichier
    strncpy(jeu->fichier_ennemis, chemin_niveau, sizeof(jeu->fichier_ennemis) - 1);
    jeu->fichier_ennemis[sizeof(jeu->fichier_ennemis) - 1] = '\0';

    int cagnotte;
    char newline;
    // Lecture de la cagnotte
    AnimerAttente(1000, "   Vérification des fonds...");
    if (fscanf(fichier, "%d%c", &cagnotte, &newline) != 2 || newline != '\n') {
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format de cagnotte invalide");
        return;
    }
    jeu->cagnotte = cagnotte;

    // Vérification du format du fichier avec ennmis
    ResoudreFichier(fichier, erreur);
    if (erreur->statut_erreur) return;

    printf("\n\n\t");
    printf(ANSI_BG_GRIS_CLAIR ANSI_TEXTE_BLEU_FONCE "      ❃  Détection des ennemis en cours ...       "ANSI_RESET"\n\n");
    BarreChargement(600);  
    printf("\n\n");
    Etudiant* etudiants = InitialisationEnnemis(fichier, jeu, erreur);
    fclose(fichier);

    if (erreur->statut_erreur) return;
    jeu->etudiants = etudiants;

    printf("◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆\n");


    printf(ANSI_TEXTE_GRIS " Appuyez sur Entrée pour continuer...\n" ANSI_RESET);
    while (getchar() != '\n');

    fflush(stdout);
    VisualiserEnnemis(jeu->etudiants);
    if (erreur->statut_erreur) return;


    printf(ANSI_TEXTE_GRIS" Appuyez sur Entrée pour continuer...\n"ANSI_RESET);
    while (getchar() != '\n');
    printf(ANSI_BG_GRIS_CLAIR ANSI_TEXTE_BLEU_FONCE "       ❃  Configuration des défenses en cours...         "ANSI_RESET"\n\n");

    // cagnotte est modifée par InitialisationTourelles et prend la valeur de la somme restante
    Tourelle* tourelles = InitialisationTourelles(&jeu->cagnotte, erreur);

    if (erreur->statut_erreur) return;
    jeu->tourelles = tourelles;

    strncpy(jeu->pseudo, pseudo, sizeof(jeu->pseudo) - 1);
    jeu->pseudo[sizeof(jeu->pseudo) - 1] = '\0';
    jeu->score = 0;
    jeu->tour = 0; 
    
    printf("\n\n");
    AnimerAttente(1000, ANSI_TEXTE_BLEU_FONCE "     Préparation terminée !"ANSI_RESET);

    printf(ANSI_TEXTE_GRIS "\n\n\tAppuyez sur Entrée pour continuer..." ANSI_RESET "");
    fflush(stdout);
    while(getchar() != '\n');
    return;

}


void LibererJeu(Jeu* jeu) {
    if (jeu == NULL) {
        return;
    }
    LibererEnnemis(jeu->etudiants);
    LibererTourelles(jeu->tourelles);
}


int main() {
    Erreur erreur;
    erreur.statut_erreur = 0;

    // initialisation de la graine pour les fonctions aléatoires
    srand((unsigned int)(uintptr_t)&erreur);

    IntroMenu();

    printf("    𓀡   Entrez votre pseudo : ");
    scanf("%49s", pseudo);
    while (getchar() != '\n');

    IntroduireJeu();


    // Boucle de parties 
    while (1) {
        char* cheminPartie = Menu(&erreur);
        if (!cheminPartie) {
            if (erreur.statut_erreur) {
                printf(ANSI_BG_ROUGE ANSI_TEXTE_BLANC "    ×  Erreur : %s\n" ANSI_RESET, erreur.msg_erreur);
                erreur.statut_erreur = 0;
            }
            printf(ANSI_TEXTE_GRIS"\n   Appuyez sur Entrée pour continuer..."ANSI_RESET);
            fflush(stdout);
            while(getchar() != '\n');
            continue;
        }

        Jeu jeu;
        char chemin[256];
        
        strncpy(chemin, cheminPartie, sizeof(chemin) - 1);
        chemin[sizeof(chemin) - 1] = '\0';


        // deux possibilités : relancer une partie sauvegardée ou bien créer une nouvelle 

        if (strncmp(chemin, "Sauvegardes/", strlen("Sauvegardes/")) == 0)
            RelancerPartie(&erreur, &jeu, chemin);
        else
            PreparerPartie(&erreur, &jeu, chemin);

        if (erreur.statut_erreur==1) {
            printf(ANSI_BG_ROUGE ANSI_TEXTE_BLANC "    ×  Erreur : %s\n" ANSI_RESET, erreur.msg_erreur);
            LibererJeu(&jeu);
            return 1;
        }

        JouerPartie(&jeu, &erreur);

        if (erreur.statut_erreur==1) {
            printf(ANSI_BG_ROUGE ANSI_TEXTE_BLANC "    ×  Erreur : %s\n" ANSI_RESET, erreur.msg_erreur);
            LibererJeu(&jeu);
            return 1;
        }

        LibererJeu(&jeu);
        printf(ANSI_TEXTE_GRIS"\nAppuyez sur Entrée pour continuer...\n"ANSI_RESET);
        while (getchar() != '\n');
    }
    return 0;
}