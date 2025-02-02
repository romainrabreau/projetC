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


void SauvegarderPartie(Jeu* jeu, Erreur *erreur) {
    char nomSauvegarde[MAX_NAME_LEN];
    printf("Entrez le nom de sauvegarde : ");
    if (fgets(nomSauvegarde, sizeof(nomSauvegarde), stdin) == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Erreur de lecture du nom de sauvegarde.");
        return;
    }
    nomSauvegarde[strcspn(nomSauvegarde, "\n")] = '\0';

    mkdir("Sauvegardes", 0755);

    // Utilisation de RecupererNom avec la structure Erreur
    char *baseName = RecupererNom(jeu->fichier_ennemis, erreur);
    if (erreur->statut_erreur) {
        return;
    }
    char cheminSauvegarde[MAX_NAME_LEN];
    snprintf(cheminSauvegarde, sizeof(cheminSauvegarde), "Sauvegardes/%s_(%s)_%s_save.txt", baseName, jeu->pseudo, nomSauvegarde);

    FILE *f = fopen(cheminSauvegarde, "w");
    if (!f) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Erreur d'ouverture du fichier de sauvegarde");
        return;
    }
    fprintf(f, "Tour %d\n", jeu->tour);
    fprintf(f, "Cagnotte %d\n", jeu->cagnotte);
    fprintf(f, "Pseudo %s\n", jeu->pseudo);

    int nbTourelles = 0;
    Tourelle *t = jeu->tourelles;
    while (t) { nbTourelles++; t = t->next; }
    fprintf(f, "NbTourelles %d\n", nbTourelles);
    t = jeu->tourelles;
    while (t) {
        fprintf(f, "Tourelle %c %d %d %d %d\n", t->type, t->pointsDeVie, t->ligne, t->position, t->prix);
        t = t->next;
    }

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
    if (erreur->statut_erreur) return; // Si une erreur est déjà présente, on ne fait rien
    if (jeu == NULL) { // Vérifie que la structure jeu est initialisée
        erreur->statut_erreur = 1; // Définit le statut d'erreur
        strcpy(erreur->msg_erreur, "Structure de jeu non initialisée"); // Copie le message d'erreur
        return; 
    }
    FILE *fichier = fopen(chemin_save, "r"); // Ouvre le fichier de sauvegarde en lecture
    if (!fichier) { // Si l'ouverture échoue
        erreur->statut_erreur = 1; // Définit le statut d'erreur
        snprintf(erreur->msg_erreur, sizeof(erreur->msg_erreur), "Ouverture impossible : %s", chemin_save); // Message d'erreur
        return;
    }
    // Copie initiale du chemin de sauvegarde dans fichier_ennemis (sera mis à jour plus tard)
    strncpy(jeu->fichier_ennemis, chemin_save, sizeof(jeu->fichier_ennemis) - 1); // Copie le chemin
    jeu->fichier_ennemis[sizeof(jeu->fichier_ennemis) - 1] = '\0'; // Termine par '\0'
    
    if (fscanf(fichier, "Tour %d\n", &jeu->tour) != 1) { // Lit le numéro du tour
        fclose(fichier); // Ferme le fichier
        erreur->statut_erreur = 1; // Définit le statut d'erreur
        strcpy(erreur->msg_erreur, "Format de tour invalide"); // Message d'erreur
        return;
    }
    if (fscanf(fichier, "Cagnotte %d\n", &jeu->cagnotte) != 1) { // Lit la cagnotte
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format de cagnotte invalide");
        return;
    }
    {   // Lit le pseudo sauvegardé et le copie dans jeu->pseudo
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
    int nbTourelles;
    if (fscanf(fichier, "NbTourelles %d\n", &nbTourelles) != 1) { // Lit le nombre de tourelles
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format du nombre de tourelles invalide");
        return;
    }
    jeu->tourelles = NULL;
    Tourelle *dernierTourelle = NULL;
    for (int i = 0; i < nbTourelles; i++) { // Parcourt toutes les tourelles
        char type;
        int pointsDeVie, ligne, position, prix;
        if (fscanf(fichier, "Tourelle %c %d %d %d %d\n", &type, &pointsDeVie, &ligne, &position, &prix) != 5) {
            fclose(fichier);
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Format d'une tourelle invalide");
            return;
        }
        Tourelle *nouvelle = malloc(sizeof(Tourelle)); // Alloue la mémoire pour une tourelle (allocation conservée)
        if (!nouvelle) {
            fclose(fichier);
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Erreur d'allocation pour tourelle");
            return;
        }
        nouvelle->type = type; nouvelle->pointsDeVie = pointsDeVie; nouvelle->ligne = ligne; nouvelle->position = position; nouvelle->prix = prix; nouvelle->next = NULL; // Initialise la tourelle
        if (jeu->tourelles == NULL)
            jeu->tourelles = nouvelle; // Si première tourelle, la rattache à jeu->tourelles
        else
            dernierTourelle->next = nouvelle; // Sinon, l'ajoute en fin de liste
        dernierTourelle = nouvelle; // Met à jour le dernier pointeur
    }
    int nbEtudiants;
    if (fscanf(fichier, "NbEtudiants %d\n", &nbEtudiants) != 1) { // Lit le nombre d'étudiants
        fclose(fichier);
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Format du nombre d'étudiants invalide");
        return;
    }
    jeu->etudiants = NULL;
    Etudiant *dernierEtudiant = NULL;
    for (int i = 0; i < nbEtudiants; i++) { // Parcourt tous les étudiants
        char type;
        int pointsDeVie, ligne, position, vitesse, tourEnnemi, immobilisation;
        if (fscanf(fichier, "Etudiant %c %d %d %d %d %d %d\n",
                   &type, &pointsDeVie, &ligne, &position, &vitesse, &tourEnnemi, &immobilisation) != 7) {
            fclose(fichier);
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Format d'un étudiant invalide");
            return;
        }
        Etudiant *nouvel = malloc(sizeof(Etudiant)); // Alloue la mémoire pour un étudiant (allocation conservée)
        if (!nouvel) {
            fclose(fichier);
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Erreur d'allocation pour étudiant");
            return;
        }
        nouvel->type = type; nouvel->pointsDeVie = pointsDeVie; nouvel->ligne = ligne; nouvel->position = position; nouvel->vitesse = vitesse; nouvel->tour = tourEnnemi; nouvel->immobilisation = immobilisation;
        nouvel->next = NULL; nouvel->next_line = NULL; nouvel->prev_line = NULL; // Initialise l'étudiant
        if (jeu->etudiants == NULL)
            jeu->etudiants = nouvel; // Si premier étudiant, rattache à jeu->etudiants
        else
            dernierEtudiant->next = nouvel; // Sinon, ajoute en fin de liste
        dernierEtudiant = nouvel; // Met à jour le dernier pointeur
    }
    fclose(fichier); // Ferme le fichier après lecture
    jeu->score = 0; // Initialise le score
    animer_attente(500, "Partie relancée !"); // Animation de fin de relance

    // Mise à jour de fichier_ennemis : on extrait le nom du fichier sans répertoires pour éviter un doublon dans le leaderboard
    {  
        char nomFichierSauvegarde[MAX_NAME_LEN] = {0}; // Déclare un buffer statique pour le nom extrait
        ExtraireNomFichierSauvegarde(chemin_save, erreur, nomFichierSauvegarde); // Remplit le buffer (fonction modifiée pour utiliser un buffer)
        strncpy(jeu->fichier_ennemis, nomFichierSauvegarde, sizeof(jeu->fichier_ennemis) - 1); // Copie le nom extrait dans jeu->fichier_ennemis
        jeu->fichier_ennemis[sizeof(jeu->fichier_ennemis) - 1] = '\0'; // Termine par '\0'
    }
    
    remove(chemin_save); // Supprime le fichier de sauvegarde pour éviter des sauvegardes infinies
}
