#include "src/header.h"

char pseudo[50]; // Variable Gloable pour le pseudo car utilisé dans plusieurs fichiers.

/*
 Libère la mémoire allouée aux ennemis et aux tourelles.
 (Ici, les fonctions LibererEnnemis et LibererTourelles gèrent elles-mêmes la libération des ressources utilisées par ces structures.)
 */
void LibererJeu(Jeu* jeu) {
    if (jeu == NULL) {
        return;
    }
    LibererEnnemis(jeu->etudiants);
    LibererTourelles(jeu->tourelles);
}

/*
 Prépare la partie à partir du fichier de niveau indiqué par chemin_niveau.
 1) Ouvre le fichier du niveau et lit la cagnotte.
 2) Initialise les ennemis via InitialisationEnnemis.
 3) Affiche les ennemis via VisualiserEnnemis.
 4) Initialise les tourelles via InitialisationTourelles.
 Le chemin du fichier est également sauvegardé dans la structure de jeu.
 */
void PreparerPartie(Erreur* erreur, Jeu* jeu, char* chemin_niveau) {
    if (erreur->statut_erreur) return;
    if (jeu == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Structure de jeu non initialisée");
        return;
    }

    // On lis le fichier fourni
    FILE* fichier = fopen(chemin_niveau, "r");
    if (!fichier) {
        erreur->statut_erreur = 1;
        snprintf(erreur->msg_erreur, sizeof(erreur->msg_erreur), "Ouverture impossible : %s", chemin_niveau);
        return;
    }

    // Sauvegarde du chemin dans la structure de jeu 
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

    /* Initialisation des ennemis */
    animer_attente(1500, "Analyse des menaces...");
    printf("\033[36;47mDétection des ennemis..." RESET "\n");
    Etudiant* etudiants = InitialisationEnnemis(fichier, jeu, erreur);
    fclose(fichier);
    if (erreur->statut_erreur) return;
    jeu->etudiants = etudiants;

    VisualiserEnnemis(jeu->etudiants, erreur);
    if (erreur->statut_erreur) return;

    /* Initialisation des tourelles */
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


// Affiche le menu principal et renvoie le chemin de la partie sélectionné par l'utilisateur, ou NULL si on affiche les classements.
char* Menu(Erreur *err) {
    // Effacer l'écran et afficher le titre.
    printf("\033[0;0H\033[2J");
    AfficherTitre();
    printf("Bienvenue %s !\n\n", pseudo);
    
    // Menu principal
    char menu_options[5][MAX_NAME_LEN] = {
        "Jouer les niveaux", 
        "Charger une partie", 
        "Reprendre une partie sauvegardée",
        "Classements", 
        "Quitter"
    };
    int choix = AfficherChoix(menu_options, 5, err); // Donne une liste de choix à l'utilisateurs pour qu'il choisisse une option au clavier.
    animer_attente(500, CLEAR_LINE); // ces deux fonctions sont dans effetsVisuels.c
    
    static char chemin[MAX_NAME_LEN];
    
    switch(choix) {
        case 0: { // Jouer les niveaux
            int nbNiveaux = 0;
            char (*noms)[MAX_NAME_LEN] = LectureNoms("Niveau", &nbNiveaux, err); // Lis les noms de fichiers texte dans le dossier Niveau.
            if (noms == NULL || nbNiveaux == 0) {
                if(err) {
                    snprintf(err->msg_erreur, sizeof(err->msg_erreur), "Aucun niveau disponible ou dossier 'Niveau' introuvable!");
                    err->statut_erreur = 1;
                }
                return NULL;
            }
            char (*options)[MAX_NAME_LEN] = FormatterNoms(noms, nbNiveaux, err); // Retourne la liste des options de niveaux dans un format lisible pour l'utilisateur
            int selection = AfficherChoix(options, nbNiveaux, err);
            if (selection < 0 || selection >= nbNiveaux)
                return NULL;
            snprintf(chemin, sizeof(chemin), "Niveau/%s", noms[selection]);
            return chemin;
        }
        case 1: { // Charger une partie (saisie manuelle)
            char buffer[MAX_NAME_LEN];
            printf("Entrez l'adresse de la partie à charger (exemple: Dossier/2_Talents_De_Sprinteur.txt) : ");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                if(err) {
                    snprintf(err->msg_erreur, sizeof(err->msg_erreur), "Erreur de lecture.");
                    err->statut_erreur = 1;
                }
                return NULL;
            }
            buffer[strcspn(buffer, "\n")] = '\0';
            if (strlen(buffer) == 0) {
                if(err) {
                    snprintf(err->msg_erreur, sizeof(err->msg_erreur), "Adresse vide, retour au menu.");
                    err->statut_erreur = 1;
                }
                return NULL;
            }
            snprintf(chemin, sizeof(chemin), "%s", buffer);
            return chemin;
        }
        case 2: { // Reprendre une partie sauvegardée
            int nbSave = 0;
            char (*noms)[MAX_NAME_LEN] = LectureNoms("Sauvegardes", &nbSave, err); // Lis les fichiers texte du dossier Sauvegardes
            if (noms == NULL || nbSave == 0) {
                if(err) {
                    snprintf(err->msg_erreur, sizeof(err->msg_erreur), "Aucune sauvegarde disponible ou dossier 'Sauvegardes' introuvable!");
                    err->statut_erreur = 1;
                }
                sleep(1);
                return NULL;
            }
            char (*options)[MAX_NAME_LEN] = FormatterNoms(noms, nbSave, err); /// Retourne la liste des options de niveaux dans un format lisible pour l'utilisateur
            // On retire le suffixe "save" pour plus de lisibilité.
            for (int i = 0; i < nbSave; i++) {
                char *pos = strstr(options[i], "save");
                if (pos)
                    *pos = '\0';
            }
            /* Ajouter l'option "Retour" à la fin */
            strncpy(options[nbSave], "Retour", MAX_NAME_LEN - 1);
            options[nbSave][MAX_NAME_LEN - 1] = '\0';
            
            int selection = AfficherChoix(options, nbSave + 1, err);
            if (selection < 0 || selection > nbSave)
                return NULL;
            if (selection == nbSave)  // Option "Retour" choisie
                return NULL;
            snprintf(chemin, sizeof(chemin), "Sauvegardes/%s", noms[selection]);
            return chemin;
        }
        case 3: { // Classements
            ChoixLeaderboard(err);
            return NULL;
        }
        case 4: { // Quitter
            exit(0);
        }
        default:
            return NULL;
    }
}

int main() {
    Erreur erreur = {0};
    IntroMenu(); // Affiche l'introduction du jeu

    printf("Entrez votre pseudo : ");
    scanf("%49s", pseudo);
    while (getchar() != '\n'); 

    while (1) { // Boucle principale entre les parties
        char* cheminPartie = Menu(&erreur); // Affiche le menu et récupère le chemin choisi
        if (!cheminPartie)
            continue;

        Jeu jeu; 
        char chemin[MAX_NAME_LEN]; // Buffer pour le chemin de la partie
        strncpy(chemin, cheminPartie, MAX_NAME_LEN - 1); // Copie le chemin
        chemin[MAX_NAME_LEN - 1] = '\0'; // Termine par '\0'

        // Relance une sauvegarde si le chemin commence par "Sauvegardes/", sinon prépare un niveau normal
        if (strncmp(chemin, "Sauvegardes/", strlen("Sauvegardes/")) == 0) {
            RelancerPartie(&erreur, &jeu, chemin);
        } else {
            PreparerPartie(&erreur, &jeu, chemin);
        }
            
        if (erreur.statut_erreur) { // En cas d'erreur, affiche le message et on libère la mémoire du jeu qui à potentiellement été initialisé.
            printf("\033[31mERREUR: %s\033[0m\n", erreur.msg_erreur);
            LibererJeu(&jeu);
            continue;
        }

        JouerPartie(&jeu, &erreur); // Démarre la partie
        LibererJeu(&jeu); // Libère la mémoire après la partie
        printf("\nAppuyez sur Entrée pour continuer...");
        while (getchar() != '\n');
    }
    return 0;
}