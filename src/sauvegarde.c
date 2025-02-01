#include "header.h"

void SauvegarderPartie(Jeu* jeu) {
    char nomSauvegarde[100];
    printf("Entrez le nom de sauvegarde : ");
    if (fgets(nomSauvegarde, sizeof(nomSauvegarde), stdin) == NULL) {
        printf("Erreur de lecture du nom de sauvegarde.\n");
        return;
    }
    nomSauvegarde[strcspn(nomSauvegarde, "\n")] = '\0';

    mkdir("Sauvegardes", 0755);

    char* baseName = RecupererNom(jeu->fichier_ennemis);
    if (!baseName) {
        printf("Erreur lors de la récupération du nom de fichier.\n");
        return;
    }
    // Format demandé : [(PseudoJoueur)]_[nomsauvegarde]_[(NomFichier)]_save.txt
    char cheminSauvegarde[256];
    snprintf(cheminSauvegarde, sizeof(cheminSauvegarde), 
             "Sauvegardes/(%s)_%s_(%s)_save.txt", 
             jeu->pseudo, nomSauvegarde, baseName);
    free(baseName);

    FILE* f = fopen(cheminSauvegarde, "w");
    if (!f) {
        perror("Erreur d'ouverture du fichier de sauvegarde");
        return;
    }
    fprintf(f, "Tour %d\n", jeu->tour);
    fprintf(f, "Cagnotte %d\n", jeu->cagnotte);
    fprintf(f, "Pseudo %s\n", jeu->pseudo);

    // Sauvegarde des tourelles
    int nbTourelles = 0;
    Tourelle* t = jeu->tourelles;
    while (t) { nbTourelles++; t = t->next; }
    fprintf(f, "NbTourelles %d\n", nbTourelles);
    t = jeu->tourelles;
    while (t) {
        // Le type est sauvegardé en tant que caractère
        fprintf(f, "Tourelle %c %d %d %d %d\n", t->type, t->pointsDeVie, t->ligne, t->position, t->prix);
        t = t->next;
    }

    // Sauvegarde des ennemis (étudiants/zombies)
    int nbEtudiants = 0;
    Etudiant* e = jeu->etudiants;
    while (e) { nbEtudiants++; e = e->next; }
    fprintf(f, "NbEtudiants %d\n", nbEtudiants);
    e = jeu->etudiants;
    while (e) {
        fprintf(f, "Etudiant %c %d %d %d %d %d %d\n", e->type, e->pointsDeVie, e->ligne, e->position, e->vitesse, e->tour, e->immobilisation);
        e = e->next;
    }
    fclose(f);
    printf("Partie sauvegardée dans %s\n", cheminSauvegarde);
}