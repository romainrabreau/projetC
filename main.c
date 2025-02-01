#include "src/header.h"

char pseudo[50];

void LibererJeu(Jeu* jeu) {
    if (jeu == NULL) {
        return;
    }
    LibererEnnemis(jeu->etudiants);
    LibererTourelles(jeu->tourelles);
}

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
                "Ouverture impossible : %s", chemin_niveau);
        return;
    }

    // Stockage du chemin
    strncpy(jeu->fichier_ennemis, chemin_niveau, sizeof(jeu->fichier_ennemis) - 1);
    jeu->fichier_ennemis[sizeof(jeu->fichier_ennemis) - 1] = '\0';

    int cagnotte;
    char newline;
    animer_attente(500, "Vérification des fonds...");
    if (fscanf(fichier, "%d%c", &cagnotte, &newline) != 2 || newline != '\n') {
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format de cagnotte invalide");
        return;
    }
    jeu->cagnotte = cagnotte;

    // Initialisation des ennemis
    animer_attente(1500, "Analyse des menaces...");
    printf("\033[36;47mDétection des ennemis..." RESET "\n");
    Etudiant* etudiants = InitialisationEnnemis(fichier, jeu, erreur);
    fclose(fichier);
    if (erreur->statut_erreur) return;
    jeu->etudiants = etudiants;

    VisualiserEnnemis(jeu->etudiants, erreur);
    if (erreur->statut_erreur) return;

    animer_attente(1000, "Configuration des défenses...");
    Tourelle* tourelles = InitialisationTourelles(&jeu->cagnotte, erreur);
    if (erreur->statut_erreur) return;
    jeu->tourelles = tourelles;

    strncpy(jeu->pseudo, pseudo, sizeof(jeu->pseudo) - 1);
    jeu->pseudo[sizeof(jeu->pseudo) - 1] = '\0';
    jeu->score = 0;
    jeu->tour = 1; 
    
    animer_attente(500, "Préparation terminée !");
}

char* Menu(void) {
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
    animer_attente(500, CLEAR_LINE);

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
                printf("\033[31mDossier 'Sauvegardes' introuvable !\033[0m\n");
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
            ChoixLeaderboard();
            return NULL;
        case 4: // Quitter
            exit(0);
        default:
            return NULL;
    }
}
    
int main() {
    Erreur erreur = {0};
    IntroMenu();

    printf("Entrez votre pseudo : ");
    scanf("%49s", pseudo);
    while (getchar() != '\n');

    while (1) { // Boucle de jeu
        char* cheminPartie = Menu();
        if (!cheminPartie)
            continue;

        Jeu jeu;
        char chemin[256];
        strncpy(chemin, cheminPartie, sizeof(chemin) - 1);
        chemin[sizeof(chemin) - 1] = '\0';
        free(cheminPartie);

        // Si le chemin commence par "Sauvegardes/", il s'agit d'une sauvegarde, sinon c'est un niveau normal.
        if (strncmp(chemin, "Sauvegardes/", strlen("Sauvegardes/")) == 0)
            RelancerPartie(&erreur, &jeu, chemin);
        else
            PreparerPartie(&erreur, &jeu, chemin);

        if (erreur.statut_erreur) {
            printf("\033[31mERREUR: %s\033[0m\n", erreur.msg_erreur);
            LibererJeu(&jeu);
            continue;
        }

        JouerPartie(&jeu, &erreur);

        LibererJeu(&jeu);
        printf("\nAppuyez sur Entrée pour continuer...");
        while (getchar() != '\n');
    }
    return 0;
}
