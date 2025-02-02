#include "header.h"

char pseudo[50];


void PreparerPartie(Erreur* erreur, Jeu* jeu, const char* chemin_niveau) {
    // Vérification des entrées
    if (erreur->statut_erreur) return;
    if (jeu == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Structure de jeu non initialisée");
        return;
    }

    // Ouverture du fichier
    FILE* fichier = fopen(chemin_niveau, "r");
    if (!fichier) {
        erreur->statut_erreur = 1;
        snprintf(erreur->msg_erreur, sizeof(erreur->msg_erreur), 
                "Impossible d'ouvrire le fichier : %s", chemin_niveau);
        return;
    }

    // Stockage du chemin du fichier
    strncpy(jeu->fichier_ennemis, chemin_niveau, sizeof(jeu->fichier_ennemis) - 1);
    jeu->fichier_ennemis[sizeof(jeu->fichier_ennemis) - 1] = '\0';

    int cagnotte;
    char newline;
    // Lecture de la cagnotte
    AnimerAttente(500, "   Vérification des fonds...");
    if (fscanf(fichier, "%d%c", &cagnotte, &newline) != 2 || newline != '\n') {
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format de cagnotte invalide");
        return;
    }
    jeu->cagnotte = cagnotte;

    printf(ANSI_BG_GRIS_CLAIR ANSI_TEXTE_BLEU_FONCE "       ❃  Détection des ennemis en cours ...         "ANSI_RESET"\n\n");

    Etudiant* etudiants = InitialisationEnnemis(fichier, jeu, erreur);
    fclose(fichier);

    if (erreur->statut_erreur) return;
    jeu->etudiants = etudiants;

    printf("◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆\n");


    printf(ANSI_TEXTE_GRIS " Appuyez sur Entrée pour continuer...\n" ANSI_RESET);
    getchar();

    VisualiserEnnemis(jeu->etudiants, erreur);
    if (erreur->statut_erreur) return;


    printf(ANSI_TEXTE_GRIS" Appuyez sur Entrée pour continuer...\n"ANSI_RESET);
    getchar();
    printf(ANSI_BG_GRIS_CLAIR ANSI_TEXTE_BLEU_FONCE "       ❃  Configuration des défenses en cours...         "ANSI_RESET"\n\n");

    Tourelle* tourelles = InitialisationTourelles(&jeu->cagnotte, erreur);

    if (erreur->statut_erreur) return;
    jeu->tourelles = tourelles;

    strncpy(jeu->pseudo, pseudo, sizeof(jeu->pseudo) - 1);
    jeu->pseudo[sizeof(jeu->pseudo) - 1] = '\0';
    jeu->score = 0;
    jeu->tour = 0; 
    
    AnimerAttente(500, ANSI_TEXTE_BLEU_FONCE "     Préparation terminée !"ANSI_RESET);

    printf(ANSI_TEXTE_GRIS "\n\n\t\t\t\t\t\t\tAppuyez sur Entrée pour continuer...]" ANSI_RESET "");
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


char* Menu(Erreur* erreur) {    
    printf("\033[0;0H\033[2J");
    AfficherTitre();
    printf("Bienvenue %s !\n\n", pseudo);

    char* options_menu[] = {
        "Jouer les niveaux", 
        "Charger une partie", 
        "Reprendre une partie sauvegardée",
        "Classements", 
        "Quitter"
    };
    int choix = AfficherChoix(options_menu, 5);
    AnimerAttente(500, CLEAR_LINE);

    switch(choix) {
        case 0: { // Jouer les niveaux
            DIR *dossier = opendir("Niveau");
            if (!dossier) {
                printf("\033[31mDossier 'Niveau' introuvable!\033[0m\n");
                return NULL;
            }
            char **noms = LectureNoms(dossier);
            closedir(dossier);
            if (!noms) return NULL;
            char **options = FormatterNoms(noms);
            int nb_niveaux = 0;
            while (options[nb_niveaux]) nb_niveaux++;
            options = realloc(options, (nb_niveaux + 2) * sizeof(char *));
            options[nb_niveaux] = strdup("Retour");
            options[nb_niveaux + 1] = NULL;
            nb_niveaux++;
            int selection = AfficherChoix(options, nb_niveaux);
            char *chemin = NULL;
            if (selection >= 0 && selection < nb_niveaux - 1) {
                chemin = malloc(strlen("Niveau/") + strlen(noms[selection]) + 1);
                if (chemin)
                    sprintf(chemin, "Niveau/%s", noms[selection]);
            }
            LibererNomsFormates(options);
            for (int i = 0; noms[i]; i++) free(noms[i]);
            free(noms);
            return chemin;
        }
        case 1: { // Charger une partie (saisie manuelle)
            char buffer[256];
            printf("Entrez l'adresse de la partie à charger (exemple: Dossier/2_Talents_De_Sprinteur.txt) : ");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                printf("\033[31mErreur de lecture.\033[0m\n");
                return NULL;
            }
            buffer[strcspn(buffer, "\n")] = '\0';
            if (strlen(buffer) == 0) {
                printf("\033[31mAdresse vide, retour au menu.\033[0m\n");
                return NULL;
            }
            return strdup(buffer);
        }
        case 2: { // Reprendre une partie sauvegardée
            DIR *dossier = opendir("Sauvegardes");
            if (!dossier) {
                printf("\033[31mDossier 'Sauvegardes' introuvable!\033[0m\n");
                return NULL;
            }
            char **noms = LectureNoms(dossier);
            closedir(dossier);
            if (!noms || !noms[0]) {
                if(noms) { for (int i = 0; noms[i]; i++) free(noms[i]); free(noms); }
                printf("Aucune sauvegarde disponible.\n");
                sleep(1);
                return NULL;
            }
            char **options = FormatterNoms(noms);
            int nb_save = 0;
            while (options[nb_save]) nb_save++;
            for (int i = 0; i < nb_save; i++) {
                char *pos = strstr(options[i], "save");
                if (pos) *pos = '\0';
            }
            options = realloc(options, (nb_save + 2) * sizeof(char *));
            options[nb_save] = strdup("Retour");
            options[nb_save + 1] = NULL;
            nb_save++;
            int selection = AfficherChoix(options, nb_save);
            char *chemin = NULL;
            if (selection >= 0 && selection < nb_save - 1) {
                chemin = malloc(strlen("Sauvegardes/") + strlen(noms[selection]) + 1);
                if (chemin)
                    sprintf(chemin, "Sauvegardes/%s", noms[selection]);
            }
            LibererNomsFormates(options);
            for (int i = 0; noms[i]; i++) free(noms[i]);
            free(noms);
            return chemin;
        }
        case 3: // Classements
            ChoixLeaderboard(erreur);
            return NULL;
        case 4: // Quitter
            exit(0);
        default:
            return NULL;
    }
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

    IntroduireJeu(&erreur);

    // Boucle de parties 
    while (1) {
        char* cheminPartie = Menu(&erreur);
        if (!cheminPartie)
            continue;
        if (erreur.statut_erreur) {
            printf(ANSI_BG_ROUGE ANSI_TEXTE_BLANC "    ×  Erreur : %s" ANSI_RESET, erreur.msg_erreur);
            continue;
        }

        Jeu jeu;
        char chemin[256];
        
        strncpy(chemin, cheminPartie, sizeof(chemin) - 1);
        chemin[sizeof(chemin) - 1] = '\0';
        free(cheminPartie);


        // deux possibilités : relancer une partie sauvegardée ou bien créer une nouvelle 

        if (strncmp(chemin, "Sauvegardes/", strlen("Sauvegardes/")) == 0)
            RelancerPartie(&erreur, &jeu, chemin);
        else
            PreparerPartie(&erreur, &jeu, chemin);

        if (erreur.statut_erreur==1) {
            printf(ANSI_BG_ROUGE ANSI_TEXTE_BLANC "    ×  Erreur : %s" ANSI_RESET, erreur.msg_erreur);
            LibererJeu(&jeu);
            continue;
        }

        JouerPartie(&jeu, &erreur);

        if (erreur.statut_erreur==1) {
            printf(ANSI_BG_ROUGE ANSI_TEXTE_BLANC "    ×  Erreur : %s" ANSI_RESET, erreur.msg_erreur);
            LibererJeu(&jeu);
            return 1;
        }

        LibererJeu(&jeu);
        printf("\nAppuyez sur Entrée pour continuer...");
        while (getchar() != '\n');
    }
    return 0;
}