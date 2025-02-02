#include "header.h"

// Cette fonction utilitaire extrait le "NomFichier" d'un chemin de sauvegarde
// Par exemple, pour "Sauvegardes/2_Talents_De_Sprinteur_(Manu)_MaSauvegarde_save.txt", la fonction renvoie "2_Talents_De_Sprinteur"
char *ExtraireNomFichierSauvegarde(const char *chemin_save, Erreur *erreur) {
    // Cherche le dernier '/' dans le chemin pour obtenir le nom complet du fichier.
    char *nomComplet = strrchr(chemin_save, '/');
    if (!nomComplet)
        nomComplet = (char *)chemin_save;
    else
        nomComplet++;
    
    // On suppose le format "NomFichier_(Pseudo)_NomAttribué_save.txt", donc on cherche le premier '_' qui sépare le NomFichier du reste.
    char *posU = strchr(nomComplet, '_');
    if (!posU) {
        // Aucun '_' trouvé : on retourne une copie de nomComplet sans l'extension
        char *copie = strdup(nomComplet);
        if (!copie) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Erreur d'allocation dans ExtraireNomFichierSauvegarde");
            return NULL;
        }
        // cherche le dernier '.' pour enlever l'extension
        char *point = strrchr(copie, '.');
        if (point)
            *point = '\0';
        return copie;
    }
    // Calcule la longueur de la partie du texte avant le premier underscore (soit le NomFichier)
    size_t len = posU - nomComplet;
    char *resultat = (char *)malloc(len + 1);
    if (!resultat) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Erreur d'allocation dans ExtraireNomFichierSauvegarde");
        return NULL;
    }
    strncpy(resultat, nomComplet, len);
    resultat[len] = '\0';
    return resultat;
}

// Sauvegarde l'état du jeu dans un fichier texte pour quel'utilisateur reprenne sa partie plus tard.
void SauvegarderPartie(Jeu* jeu, Erreur *erreur) {
    // On demande à l'utilisateur de nommer sa sauvegarde comme ça il peut en avoir plusieurs pour un même niveau
    char nomSauvegarde[100];
    printf("Entrez le nom de sauvegarde :   ");
    if (fgets(nomSauvegarde, sizeof(nomSauvegarde), stdin) == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Mauvais nom de sauvegarde entré");
        return;
    }
    nomSauvegarde[strcspn(nomSauvegarde, "\n")] = '\0';
    
    mkdir("Sauvegardes", 0755); // On teste si le dossier existe, sinon on le créé

    // On isole le nom du nievau pour composer le nom de la sauvegarde (format ci dessous)
    char *baseName = RecupererNom(jeu->fichier_ennemis);
    if (!baseName) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Erreur lors de la récupération du nom de fichier");
        return;
    }

    // Format : NomFichier_(Pseudo)_NomAttribué_save.txt, ce qui permet d'être lu par Formatter Noms pour afficher correctement les selections de niveaux ou autres à différents moments.
    char cheminSauvegarde[250];
    
    snprintf(cheminSauvegarde, sizeof(cheminSauvegarde), "Sauvegardes/%s_(%s)_%s_save.txt", baseName, jeu->pseudo, nomSauvegarde);
    free(baseName);

    // On écrit dans le fichier texte créé toutes les stats du jeu en cours, selon le format dans les printf.
    FILE *f = fopen(cheminSauvegarde, "w");
    if (!f) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Erreur d'ouverture du fichier de sauvegarde");
        return;
    }
    fprintf(f, "Tour %d\n", jeu->tour);
    fprintf(f, "Cagnotte %d\n", jeu->cagnotte);
    fprintf(f, "Pseudo %s\n", jeu->pseudo);

    // Sauvegarde des tourelles
    int nbTourelles = 0;
    Tourelle *t = jeu->tourelles;
    // d'abord on parcourt les tourelles pour les compter
    while (t) { nbTourelles++; t = t->next; }
    fprintf(f, "NbTourelles %d\n", nbTourelles);
    // ensuite une par une pour les sauvegarder dans le fichier txt créé
    t = jeu->tourelles;
    while (t) {
        fprintf(f, "Tourelle %c %d %d %d %d\n", t->type, t->pointsDeVie, t->ligne, t->position, t->prix);
        t = t->next;
    }

    // Sauvegarde des étudiants
    int nbEtudiants = 0;
    Etudiant *e = jeu->etudiants;
    while (e) { nbEtudiants++; e = e->next; }
    fprintf(f, "NbEtudiants %d\n", nbEtudiants);
    e = jeu->etudiants;
    while (e) {
        fprintf(f, "Etudiant %c %d %d %d %d %d %d\n",
                e->type, e->pointsDeVie, e->ligne, e->position, e->vitesse, e->tour, e->immobilisation);
        e = e->next;
    }
    fclose(f);
    printf("Partie sauvegardée dans %s\n", cheminSauvegarde);
}


// Cette fonction reprend une partie sauvegardée en lisant le fichier de sauvegarde et en reconstituant l'état du jeu.
// 1) On remet les stats de la partie, puis on relance la partie en appellant JouerPartie.
// 2) Après la relance, le champ fichier_ennemis est mis à jour pour ne contenir que le NomFichier extrait du chemin de sauvegarde (via ExtraireNomFichierSauvegarde).
// Cela permet de ne pas créer un leaderboard doublon car la partie aurait repris avec un nouveau nom...
// 3) Enfin, le fichier de sauvegarde est supprimé, c'est pour éviter de créer des sauvegardes à l'infini.
void RelancerPartie(Erreur* erreur, Jeu* jeu, const char* chemin_save) {
    if (erreur->statut_erreur)
        return;
    if (jeu == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Structure de jeu non initialisée");
        return;
    }
    // On ouvre le fichier et on lit toutes les stats pour les attribuer à la structure Jeu.
    FILE *fichier = fopen(chemin_save, "r");
    if (!fichier) {
        erreur->statut_erreur = 1;
        snprintf(erreur->msg_erreur, sizeof(erreur->msg_erreur), "Ouverture impossible de : %s", chemin_save);
        return;
    }

    strncpy(jeu->fichier_ennemis, chemin_save, sizeof(jeu->fichier_ennemis) - 1);
    jeu->fichier_ennemis[sizeof(jeu->fichier_ennemis) - 1] = '\0';

    // Lecture du tour courant du jeu ..
    if (fscanf(fichier, "Tour %d\n", &jeu->tour) != 1) {
        // on a pas trouvé le nombre correspondant au tour
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format de tour invalide");
        return;
    }
    // ..de la cagnotte..
    if (fscanf(fichier, "Cagnotte %d\n", &jeu->cagnotte) != 1) {
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format de cagnotte invalide");
        return;
    }
    // ..du pseudo du joueur qui a fait la partie. On ne reprend pas le pseudo demandé au lancement du code car ce n'est pas lui qui a commencé la partie.
    {
        char pseudoSauvegarde[50];
        // match le format nécessaire pour le pseudo   
        if (fscanf(fichier, "Pseudo %49[^\n]\n", pseudoSauvegarde) != 1) {
            fclose(fichier);
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Format de pseudo invalide");
            return;
        }
        strncpy(jeu->pseudo, pseudoSauvegarde, sizeof(jeu->pseudo) - 1);
        jeu->pseudo[sizeof(jeu->pseudo) - 1] = '\0';
    }
    // On récupère les stats sur les tourelles.
    int nbTourelles;
    if (fscanf(fichier, "NbTourelles %d\n", &nbTourelles) != 1) {
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format du nombre de tourelles invalide");
        return;
    }

    jeu->tourelles = NULL;
    Tourelle *dernierTourelle = NULL;

    for (int i = 0; i < nbTourelles; i++) {
        char type;
        int pointsDeVie, ligne, position, prix;
        if (fscanf(fichier, "Tourelle %c %d %d %d %d\n", &type, &pointsDeVie, &ligne, &position, &prix) != 5) {
            fclose(fichier);
            LibererTourelles(jeu->tourelles);
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Format d'une tourelle invalide");
            return;
        }
        Tourelle *nouvelle = malloc(sizeof(Tourelle));
        if (!nouvelle) {
            LibererTourelles(jeu->tourelles);
            fclose(fichier);
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Erreur d'allocation pour tourelle");
            return;
        }
        nouvelle->type = type;
        nouvelle->pointsDeVie = pointsDeVie;
        nouvelle->ligne = ligne;
        nouvelle->position = position;
        nouvelle->prix = prix;
        nouvelle->next = NULL;
        if (jeu->tourelles == NULL) 
            jeu->tourelles = nouvelle;
        else 
            dernierTourelle->next = nouvelle;

        dernierTourelle = nouvelle;
    }

    // On récupère les stats sur les étudiants.
    int nbEtudiants;
    if (fscanf(fichier, "NbEtudiants %d\n", &nbEtudiants) != 1) {
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format du nombre d'étudiants invalide");
        return;
    }

    jeu->etudiants = NULL;
    Etudiant *dernierEtudiant = NULL;
    for (int i = 0; i < nbEtudiants; i++) {
        char type;
        int pointsDeVie, ligne, position, vitesse, tourEnnemi, immobilisation;

        if (fscanf(fichier, "Etudiant %c %d %d %d %d %d %d\n",
                   &type, &pointsDeVie, &ligne, &position, &vitesse, &tourEnnemi, &immobilisation) != 7) {
            fclose(fichier);
            LibererEnnemis(jeu->etudiants);
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Format d'un étudiant invalide");
            return;
        }
        Etudiant *nouvel = malloc(sizeof(Etudiant));
        if (!nouvel) {
            fclose(fichier);
            LibererEnnemis(jeu->etudiants);
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Erreur d'allocation pour étudiant");
            return;
        }
        nouvel->type = type;
        nouvel->pointsDeVie = pointsDeVie;
        nouvel->ligne = ligne;
        nouvel->position = position;
        nouvel->vitesse = vitesse;
        nouvel->tour = tourEnnemi;
        nouvel->immobilisation = immobilisation;
        nouvel->next = NULL;
        nouvel->next_line = NULL;
        nouvel->prev_line = NULL;

        if (jeu->etudiants == NULL)
            jeu->etudiants = nouvel;
        else
            dernierEtudiant->next = nouvel;
        dernierEtudiant = nouvel;
    }

    fclose(fichier);
    jeu->score = 0;
    AnimerAttente(500, "   Partie relancée !");

    // Mise à jour de fichier_ennemis pour ne conserver que le NomFichier extrait de la sauvegarde.
    // Cela permet de ne pas créer un leaderboard doublon car la partie aurait repris avec un nouveau nom...
    char *nomFichierSauvegarde = ExtraireNomFichierSauvegarde(chemin_save, erreur);

    strncpy(jeu->fichier_ennemis, nomFichierSauvegarde, sizeof(jeu->fichier_ennemis) - 1);
    jeu->fichier_ennemis[sizeof(jeu->fichier_ennemis) - 1] = '\0';
    free(nomFichierSauvegarde);

    // Suppression du fichier de sauvegarde une fois la partie relancée pour ne pas en créer trop
    remove(chemin_save);
}