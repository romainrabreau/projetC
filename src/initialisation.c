#include "header.h"

void IntroduireJeu() {
    printf("\n\n\n");
    printAvecDelai("        Cher membre de l’administration, bienvenue dans Licence vs Étudiant, le jeu dans lequel l’Université " ANSI_BG_BLEU_MEGA_LIGHT ANSI_TEXTE_BLEU_MOYEN "PSL -  Pixel Sciences et Logique" ANSI_RESET" est sous attaque ! \n", 15);
    printAvecDelai("        Votre but sera d’empêcher les Étudiants d’atteindre la Licence.\n", 15);
    printf(ANSI_TEXTE_GRIS"\n   Appuyez sur Entrée pour continuer..."ANSI_RESET);
    fflush(stdout);
    while(getchar() != '\n');
    printf("\n\n");
    printAvecDelai("\tIls apparaissent avec retard, effectuent des rattrapages, et contestent les moyennes… C’est le chaos ! \n\n", 15);
    printAvecDelai("\tArmé de votre budget " ANSI_TEXTE_BLEU_MOYEN"limité" ANSI_RESET" , vous devrez placer judicieusement vos défenses pour maintenir l’ordre académique.\n", 20);

    printf(ANSI_TEXTE_GRIS"\n   Appuyez sur Entrée pour continuer..."ANSI_RESET);
    while(getchar() != '\n');
    printf("\n\n");
    printAvecDelai("\tLes étudiants apparaissent en plusieurs tours, chaque vague plus déterminée que la précédente.\n", 20);
    printAvecDelai("\tAvec une bonne stratégie à laquelle vous conformerez avec rigueur votre administration, vous pourrez les stopper avant qu’ils n’atteignent leur objectif.\n", 20);
    printf("\n");
    printAvecDelai(ANSI_TEXTE_BLEU_FONCE"\tBonne chance, que la bureaucratie soit avec vous !" ANSI_RESET, 30);
    printf("\n\n");
    printf(ANSI_TEXTE_GRIS"\n   Appuyez sur Entrée pour continuer..."ANSI_RESET);
    while(getchar() != '\n');

    printf("\n\n\n");
    printf(ANSI_TEXTE_BLEU_FONCE "\t\tChargement du Menu..\n." ANSI_RESET);
    printf("\n");
    BarreChargement(500); 
    return;
}



void IntroMenu() {
    printf("\033[0;0H\033[2J");
    printf("\n\n\t\t");
    printAvecDelai(ANSI_TEXTE_BLEU_FONCE "Bienvenue dans 'Licence vs Étudiants'.\n" ANSI_RESET, 50);
    printf("\n");
}

// Affiche le menu principal et renvoie le chemin de la partie sélectionné par l'utilisateur, 
// ou NULL si on affiche les classements
char* Menu(Erreur* erreur) {    
    // Effacer l'écran et afficher le titre
    printf("\033[0;0H\033[2J");
    AfficherTitre();
    printf("\tBienvenue %s !\n\n", pseudo);
    // Menu principal
    char menu_options[5][MAX_NAME_LEN] = {
        {"Jouer les niveaux"}, 
        {"Charger une partie"}, 
        {"Reprendre une partie sauvegardée"},
        {"Classements"}, 
        {"Quitter"}
    };
    int choix = AfficherChoix(menu_options, 5, erreur); 
    if (erreur->statut_erreur) {
        return NULL;
    }

    printf("\n\n");
    printAvecDelai("\t\t\t\t\t\t\t" ANSI_BG_BLEU_FONCE "                                          ", 15);
    printf(ANSI_RESET"\n\n");

    static char chemin[MAX_NAME_LEN];

    switch(choix) {
        case 0: { // Jouer les niveaux
            int nbNiveaux = 0;
            char (*noms)[MAX_NAME_LEN] = LectureNoms("Niveau", &nbNiveaux, erreur); // Lis les noms de fichiers texte dans le dossier Niveau.
            if (noms == NULL || nbNiveaux == 0) {
                erreur->statut_erreur = 1;
                strcpy(erreur->msg_erreur, "Aucun niveau disponible ou dossier 'Niveau' introuvable!");
                return NULL;
            }
            char (*options)[MAX_NAME_LEN] = FormatterNoms(noms, nbNiveaux, erreur); // Retourne la liste des options de niveaux dans un format lisible pour l'utilisateur
            int selection = AfficherChoix(options, nbNiveaux, erreur);
            if (selection < 0 || selection >= nbNiveaux) {
                strcpy(erreur->msg_erreur, "Aucun niveau valide trouvé.");
                erreur->statut_erreur = 1;
                return NULL;

            }
            snprintf(chemin, sizeof(chemin), "Niveau/%s", noms[selection]);
            return chemin;
        }
        case 1: { // Charger une partie (saisie manuelle)
            char buffer[MAX_NAME_LEN];
            printf("Entrez l'adresse de la partie à charger (exemple: Dossier/2_Talents_De_Sprinteur.txt) : ");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                strcpy(erreur->msg_erreur, "Erreur de lecture.");
                erreur->statut_erreur = 1;
                return NULL;
            }
            buffer[strcspn(buffer, "\n")] = '\0';
            if (strlen(buffer) == 0) {
                strcpy(erreur->msg_erreur, "Adresse vide, retour au menu.");
                erreur->statut_erreur = 1;
                return NULL;
            }
            snprintf(chemin, sizeof(chemin), "%s", buffer);
            return chemin;
        }
        case 2: { // Reprendre une partie sauvegardée
            int nbSave = 0;
            char (*noms)[MAX_NAME_LEN] = LectureNoms("Sauvegardes", &nbSave, erreur); // Lis les fichiers texte du dossier Sauvegardes
            if (noms == NULL || nbSave == 0) {
                strcpy(erreur->msg_erreur, "Aucune sauvegarde disponible ou dossier 'Sauvegardes' introuvable!");
                erreur->statut_erreur = 1;
                sleep(1);
                return NULL;
            }
            char (*options)[MAX_NAME_LEN] = FormatterNoms(noms, nbSave, erreur); /// Retourne la liste des options de niveaux dans un format lisible pour l'utilisateur
            // On retire le suffixe "save" pour plus de lisibilité.
            for (int i = 0; i < nbSave; i++) {
                char *pos = strstr(options[i], "save");
                if (pos)
                    *pos = '\0';
            }
            /* Ajouter l'option "Retour" à la fin */
            strncpy(options[nbSave], "Retour", MAX_NAME_LEN - 1);
            options[nbSave][MAX_NAME_LEN - 1] = '\0';

            int selection = AfficherChoix(options, nbSave + 1, erreur);
            if (selection < 0 || selection > nbSave)
                return NULL;
            if (selection == nbSave)  // Option "Retour" choisie
                return NULL;
            snprintf(chemin, sizeof(chemin), "Sauvegardes/%s", noms[selection]);
            return chemin;
        }
        case 3: { // Classements
            ChoixLeaderboard(erreur);
            return NULL;
        }
        case 4: { // Quitter
            exit(0);
        }
        default:
            return NULL;
    }
}