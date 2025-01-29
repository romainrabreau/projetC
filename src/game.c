#include "header.h"

void ApparitionEnnemis(Jeu* jeu, Erreur* erreur) {
    Etudiant* e = jeu->etudiants;

    while (e != NULL) {
        if (e->tour == jeu->tour) {
            // Vérifie si l'étudiant précédent sur la même ligne occupe déjà la case d'apparition
            int dejaOccupe = 0;
            if (e->prev_line != NULL && 
                e->prev_line->position == (NB_EMPLACEMENTS + 1) &&
                e->prev_line->pointsDeVie > 0) 
            {
                dejaOccupe = 1;
            }
            if (dejaOccupe) {
                e->tour++;  // Décale l'apparition au tour suivant
            } else {
                e->position = NB_EMPLACEMENTS + 1;  // Positionne à la case d'apparition
            }
        }
        e = e->next;
    }
}


void ResoudreActionsTourelles(Jeu* jeu, Erreur* erreur) {
    // inflige des dégâts aux ennemis en fonction du type de tourelle
    Tourelle* t = jeu->tourelles;
    while (t != NULL) {
        if (t->pointsDeVie <= 0) {
            // la tourelle est morte
            t = t->next;
            continue;
        }
        Etudiant* e = jeu->etudiants;
        if (e == NULL) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "pas d'ennemis à attaquer");
        }

        while (e != NULL && !(e->ligne == t->ligne && e->pointsDeVie > 0 && e->position <= NB_EMPLACEMENTS + 1 && e->position > t->position)) {
            // si l'ennemi est mort, ou derrière la tourelle
                e = e->next;
        }
        if (e == NULL) {
            t = t->next;
            // pas d'ennemis à attaquer pour cette tourelle
            continue;
        }
        if ((char)t->type == 'T') {
            // tourelle de type basique
            e->pointsDeVie -= 1;
        }
        if ((char)t->type ==  'L' && t->position == e->position - 1) {
            // tourelle diplôme LSO mine
            e->pointsDeVie = 0;
            t->pointsDeVie = 0;
        }
        if ((char)t->type == 'B') {
            // tourelle BU mur de défense, ne fait rien à l'ennemi
        }
        if ((char)t->type == 'F') { // Trop fort...
            // tourelle feuille de présence, immobilise l'ennemi pendant 2 tours
            e->immobilisation = 2;
    
        }
        if ((char)t->type == 'A') {
            // tourelle amphi 4, bloque tous les ennemis de la ligne pendant 1 tour
            Etudiant* e2 = jeu->etudiants;
            while (e2 != NULL) {
                if (e2->ligne == t->ligne && e2->position <= NB_EMPLACEMENTS && e->position > t->position) {
                    e2->immobilisation = 1;
                }
                e2 = e2->next;
            }
        }
        if ((char)t->type == 'E') {
            // Tourelle E Lazard
            Etudiant* e2 = jeu->etudiants;
            while (e2 != NULL) {
                if ((e2->ligne >= t->ligne - 1 && e2->ligne <= t->ligne + 1) &&
                    (e2->position <= t->position + 4) &&
                     e->position > t->position) {

                    e2->pointsDeVie -= 1;
                }
                e2 = e2->next;
            }
        }
        if (e->pointsDeVie<=0) {
            SupprimerEnnemi(jeu, erreur, e);
        }
        //TODO si tourelles spéciales implémenter
        t = t->next;
    }
    return;
}

void ResoudreActionsEnnemis(Jeu* jeu, Erreur* erreur) {
    Etudiant* e = jeu->etudiants;
    while (e != NULL) {
        if (e->pointsDeVie <= 0 || e->position > NB_EMPLACEMENTS + 1) { // Prendre en compte les étudiants qui viennent de rentrer.
            e = e->next;
            continue;
        }
        Tourelle* t = jeu->tourelles;
        if (t == NULL) {
            return;
        }

        if ((char)e->type == 'D'){
            // Trouver les étudiants sur la même ligne
            Etudiant* e2 = jeu->etudiants;
            while (e2 != NULL) {
                if (e2->ligne == e->ligne &&
                    e2->position <= NB_EMPLACEMENTS) {
                    // Trouver les pv Max de l'étudiants
                    int pvMax;
                    for (int i = 0; i < NB_TYPES_ENNEMIS; i++) {
                        if (TYPES_ENNEMIS[i].symbole == e2->type) {
                                pvMax = TYPES_ENNEMIS[i].pointsDeVie;
                        }
                    }
                    // Soigner l'étudiant dans la limite de ses pvMax
                    if (e2->pointsDeVie < pvMax){
                        e2->pointsDeVie++;
                    }
                }
                e2 = e2->next;
            }
        }
        // on trouve la tourelle que va attaquer l'ennemi, pour les ennemis qui attaquent celle devant eux
        while (t != NULL && !(t->ligne == e->ligne && t->pointsDeVie > 0 && e->position == t->position + 1)) {
            // si la tourelle est morte, ou pas direct devant l'ennemi, ou sur une ligne différente
            t = t->next;
        }
        if (t == NULL) {
            e = e->next;
            continue; 
        }
        if ((char)e->type == 'Z' ||
            (char)e->type == 'S' ||
            (char)e->type == 'D' ||
            (char)e->type == 'A' ) {
            t->pointsDeVie -= 1;
        }
        if ((char)e->type == 'M') {
            t->pointsDeVie -= 3;
        }

        e = e->next;
    }
    return;
}


void DeplacerEnnemis(Jeu* jeu, Erreur* erreur) {
    // déplace les ennemis en fonction de leur vitesse, prend en compte les collisions
    Etudiant* e = jeu->etudiants;
    while (e) {
        // si l'ennemi est mort
        if (e->pointsDeVie <= 0 || e->position > NB_EMPLACEMENTS +1) {
            e = e->next;
            continue;
        }
        if (e->immobilisation > 0) {
            e->immobilisation--;
            e = e->next;
            continue;
        }
        int deplacement = e->vitesse;

        // vérification si il y a une attaque de tourelle qui définit la vitesse
        Tourelle* t1 = jeu->tourelles;
        while (t1) {
            if (t1->ligne == e->ligne && t1->pointsDeVie > 0 && t1->position < e->position && e->prev_line == NULL) {
                if ((char)t1->type == 'R') { // tourelle eduroam
                    deplacement = 1;
                }
            }
            t1 = t1->next;
        }
        // vérification ennemi devant
        if (e->prev_line != NULL){
            // plus on est loin, plus la position est grande
            int diff = (e->position - deplacement) - e->prev_line->position ;
            // si l'ennemi est trop proche de celui de devant
            if (diff <= 0 ){
                deplacement = e->position - (e->prev_line->position + 1);
            }
        }
        // on doit vérifier si une tourelle ne bloque pas le passage
        Tourelle* t = jeu->tourelles;
        while (t) {
            // vérifie si la tourelle est sur la même ligne, si elle est vivante, si elle est devant l'ennemi et si elle est sur la trajectoire
            if (t->ligne == e->ligne && t->pointsDeVie > 0 && t->position < e->position && t->position >= e->position - deplacement) {
                int deplacement_possible = e->position - t->position - 1;
                if (deplacement_possible < deplacement) {
                    // on garde le pire cas
                    deplacement = deplacement_possible;
                }
            }
            t = t->next;
        }
        e->position -= deplacement;
        e = e->next;
    }
}

int PartiePerdue(Jeu* jeu) {
    Etudiant* e = jeu->etudiants;
    while (e) {
        // si l'ennemi a atteint le bout de la ligne
        if(e->position <= 0) return 1;
        e = e->next;
    }
    return 0;
}

int PartieGagnee(Jeu* jeu) {
    Etudiant* e = jeu->etudiants;
    while (e) {
        if (e->pointsDeVie > 0) {
            return 0; // il reste un enneli en vie
        }
        e = e->next;
    }
    return 1; // victoire
}

void JouerPartie(Jeu* jeu, Erreur* err) {
    while(255) {
        jeu->tour++;

        ApparitionEnnemis(jeu, err);
        if(err->statut_erreur) return;

        // Clean + Afficher le plateau
        printf("\033[0;0H"); 
        printf("\033[2J");
        printf("Début du tour %d\n", jeu->tour);
        AfficherPlateau(jeu);
        printf("Appuyez sur Entrée pour continuer...\n");
        while ((getchar()) != '\n');

        ResoudreActionsTourelles(jeu, err);
        if(err->statut_erreur) return;

        ResoudreActionsEnnemis(jeu, err);
        if(err->statut_erreur) return;

        DeplacerEnnemis(jeu, err);
        if(err->statut_erreur) return;

        if(PartiePerdue(jeu)) {
            printf("\033[0;0H"); 
            printf("\033[2J");
            printf("Fin de Partie \n");
            AfficherPlateau(jeu);
            printf("Vous avez perdu... Les étudiants ont pris l'université.\n");
            break;
        }

        if(PartieGagnee(jeu)) {
            printf("\033[0;0H"); 
            printf("\033[2J");
            printf("Fin de Partie \n");
            AfficherPlateau(jeu);
            printf("Bravo, vous avez défendu l'université !\n");
            break;
        }
        
        printf("Fin du tour %d\n", jeu->tour);
        printf("Appuyez sur Entrée pour continuer...\n");
        while ((getchar()) != '\n');
    }
}