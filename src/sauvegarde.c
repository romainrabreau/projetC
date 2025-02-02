#include "header.h"

// Cette fonction utilitaire extrait le "NomFichier" d'un chemin de sauvegarde.
// Par exemple, pour "Sauvegardes/2_Talents_De_Sprinteur_(Manu)_MaSauvegarde_save.txt", la fonction renvoie "2_Talents_De_Sprinteur".
void ExtraireNomFichierSauvegarde(const char *chemin_save, Erreur *erreur, char *dest) {
    // Cherche le dernier '/' dans le chemin pour obtenir le nom complet du fichier.
    const char *nomComplet = strrchr(chemin_save, '/');
    if (!nomComplet)
        nomComplet = chemin_save;
    else
        nomComplet++;  // passe le '/'

    // On suppose avoir le format "NomFichier_(Pseudo)_NomAttribué_save.txt"
    // On cherche le premier '_' qui sépare le NomFichier du reste.
    const char *posU = strchr(nomComplet, '_');
    if (!posU) {
        // Aucun '_' trouvé : on copie nomComplet sans l'extension.
        strncpy(dest, nomComplet, MAX_NAME_LEN - 1);
        dest[MAX_NAME_LEN - 1] = '\0';
        char *point = strrchr(dest, '.');
        if (point)
            *point = '\0';
        return;
    }
    // Calcul de la longueur de la partie avant le premier underscore (soit le NomFichier)
    size_t len = posU - nomComplet;
    if (len > MAX_NAME_LEN - 1)
        len = MAX_NAME_LEN - 1;
    strncpy(dest, nomComplet, len);
    dest[len] = '\0';
}


// Sauvegarde l'état du jeu dans un fichier texte pour quel'utilisateur reprenne sa partie plus tard.
void SauvegarderPartie(Jeu* jeu, Erreur *erreur) {
    // On demande à l'utilisateur de nommer sa sauvegarde comme ça il peut en avoir plusieurs pour une même game.
    char nomSauvegarde[100];
    printf("Entrez le nom de sauvegarde : ");
    if (fgets(nomSauvegarde, sizeof(nomSauvegarde), stdin) == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Erreur de lecture du nom de sauvegarde.");
        return;
    }
    nomSauvegarde[strcspn(nomSauvegarde, "\n")] = '\0';

    mkdir("Sauvegardes", 0755); // On teste si le dossier existe, sinon on le créer.
    // On isole le nom du nievau pour composer le nom de la sauvegarde (format ci dessous)
    char *baseName = RecupererNom(jeu->fichier_ennemis);
    if (!baseName) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Erreur lors de la récupération du nom de fichier.");
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
    while (t) { nbTourelles++; t = t->next; }
    fprintf(f, "NbTourelles %d\n", nbTourelles);
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
        snprintf(erreur->msg_erreur, sizeof(erreur->msg_erreur), "Ouverture impossible : %s", chemin_save);
        return;
    }
    strncpy(jeu->fichier_ennemis, chemin_save, sizeof(jeu->fichier_ennemis) - 1);
    jeu->fichier_ennemis[sizeof(jeu->fichier_ennemis) - 1] = '\0';
    
    // Lecture du tour courant du jeu ..
    if (fscanf(fichier, "Tour %d\n", &jeu->tour) != 1) {
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format de tour invalide");
        return;
    }
    // .. de la cagnotte ..
    if (fscanf(fichier, "Cagnotte %d\n", &jeu->cagnotte) != 1) {
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format de cagnotte invalide");
        return;
    }
    // .. du pseudo du joueur qui a fait la partie.
    {
        char pseudoSauvegarde[50];
        if (fscanf(fichier, "Pseudo %49[^\n]\n", pseudoSauvegarde) != 1) {
            fclose(fichier);
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Format de pseudo invalide");
            return;
        }
        strncpy(jeu->pseudo, pseudoSauvegarde, sizeof(jeu->pseudo) - 1);
        jeu->pseudo[sizeof(jeu->pseudo) - 1] = '\0';
    }
    // Récupération des statistiques des tourelles.
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
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Format d'une tourelle invalide");
            return;
        }
        Tourelle *nouvelle = malloc(sizeof(Tourelle));
        if (!nouvelle) {
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

    // Récupération des statistiques des étudiants.
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
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Format d'un étudiant invalide");
            return;
        }
        Etudiant *nouvel = malloc(sizeof(Etudiant));
        if (!nouvel) {
            fclose(fichier);
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
    animer_attente(500, "Partie relancée !");

    /* Mise à jour de fichier_ennemis pour ne conserver que le nom du fichier extrait de la sauvegarde.
       Pour éviter la création d'un leaderboard doublon, on extrait le nom du fichier sans les répertoires.
       Ici, nous utilisons un buffer statique pour stocker le nom extrait.
       On suppose que la fonction ExtraireNomFichierSauvegarde a été modifiée pour remplir un buffer passé en argument.
    */
    {
        char nomFichierSauvegarde[MAX_NAME_LEN] = {0};
        ExtraireNomFichierSauvegarde(chemin_save, erreur, nomFichierSauvegarde);
        strncpy(jeu->fichier_ennemis, nomFichierSauvegarde, sizeof(jeu->fichier_ennemis) - 1);
        jeu->fichier_ennemis[sizeof(jeu->fichier_ennemis) - 1] = '\0';
    }

    // Suppression du fichier de sauvegarde pour éviter d'en créer trop.
    remove(chemin_save);
}
