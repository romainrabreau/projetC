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
        if ((char)t->type ==  'D' && t->position == e->position - 1) {
            // tourelle diplôme LSO mine
            e->pointsDeVie = 0;
            t->pointsDeVie = 0;
        }
        if ((char)t->type == 'B') {
            // tourelle BU mur de défense
            // ne fait rien à l'ennemi
        }
        if ((char)t->type == 'P') {
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
            // Tourelle E Lazard, dégat de zone
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

        if ((char)t->type != 'B' && (char)e->type == 'S') {
            // syndicaliste touché par la tourelle
            e->touche = 1;
        }
        if (e->pointsDeVie<=0 && (char)e->type != 'S') {
            // on ne supprime pas le syndicaliste pour pouvoir faire avancer les autres
            SupprimerEnnemi(jeu, erreur, e);
        }
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
        // on trouve la tourelle que va attaquer l'ennemi, pour les ennemis qui attaquent celle devant eux
        while (t != NULL && !(t->ligne == e->ligne && t->pointsDeVie > 0 && e->position == t->position + 1)) {
            // si la tourelle est morte, ou pas direct devant l'ennemi, ou sur une ligne différente
            t = t->next;
        }
        if (t == NULL) {
            e = e->next;
            continue; 
        }
        if ((char)e->type == 'Z' || (char)e->type == 'X' || (char)e->type == 'S') {
            // type de base
            t->pointsDeVie -= 1;
        }
        if ((char)e->type == 'L') {
            // gros dégâts, résistant mais très lent
            t->pointsDeVie -= 3;
        }
        if ((char)e->type == 'F'){
            // fainéant, fait des sauts aléatoires ou ne bouge pas pendant plusieurs tours, résistant
            int choix = rand() % 2;
            if (choix == 0) {
                t->pointsDeVie -= 1;
            }
        }
        if ((char)e->type == 'S') {
            // syndicaliste
            t->pointsDeVie -= 1;
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

        if (e->next_line && (char)e->next_line->type == 'S' && e->next_line->touche == 1) {
            // syndicaliste touché, augmente la vitesse de l'ennemi devant
            deplacement++;
            e->next_line->touche = 0;
            if (e->next_line->pointsDeVie <= 0) {
                // on supprime le syndicaliste qui est mort à l'étape précédente
                SupprimerEnnemi(jeu, erreur, e->next_line);
            }
        }

        if ((char)e->type == 'F') {
            // Fainéant : comportement aléatoire
            int choix = rand() % 6; 

            if (choix == 0) {
                // saute sur la ligne du dessus 
                if (e->ligne > 1) {
                    ChangerLigne(jeu, e, -1);
                    printf("Le Fainéant a sauté sur la ligne du dessus.\n");
                }
            } else if (choix == 2) {
                // saute sur la ligne en dessous d
                if (e->ligne < NB_LIGNES) {
                    ChangerLigne(jeu, e, 1);
                    printf("Le Fainéant a sauté sur la ligne en dessous.\n");
                }
            } else {
                // Ne fait rien (2/3)
                printf("Le Fainéant reste immobile ce tour.\n");
            }

            e = e->next;  // Passe à l'ennemi suivant
        }


        // vérification si il y a une attaque de tourelle qui définit la vitesse
        Tourelle* t1 = jeu->tourelles;
        while (t1) {
            // si pour cette tourelle l'ennemi est celui qu'elle devrai attaquer
            if (t1->ligne == e->ligne && t1->pointsDeVie > 0 && t1->position < e->position && e->prev_line == NULL) {
                if ((char)t1->type == 'R') { // tourelle eduroam
                    int choix = rand() % 3;
                    switch (choix) {
                        case 0:
                            deplacement = 0;
                            break;
                        case 1:
                            deplacement = 1;
                            break;
                        case 2:
                            deplacement = -1;
                            break;
                    }
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
        // vérification ennemi derrière
        if (e->next_line != NULL){
            int diff = e->next_line->position - (e->position - deplacement);
            // si l'ennemi est trop proche de celui de derrière
            if (diff <= 0 ){
                deplacement = e->next_line->position - (e->position - 1);
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