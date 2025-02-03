#include "header.h"

void ApparitionEnnemis(Jeu* jeu, Erreur* erreur) {
    Etudiant* e = jeu->etudiants;

    while (e != NULL) {
        if (e->tour == jeu->tour) {
            // vérifie si l'étudiant précédent sur la même ligne occupe déjà la case d'apparition
            int dejaOccupe = 0;
            if (e->prev_line != NULL && 
                e->prev_line->position == (NB_EMPLACEMENTS + 1) &&
                e->prev_line->pointsDeVie > 0) 
            {
                dejaOccupe = 1;
            }
            if (dejaOccupe) {
                e->tour++;  // décale l'apparition au tour suivant
            } else {
                e->position = NB_EMPLACEMENTS + 1;  // case d'apparition
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
            return;
        }

        while (e != NULL && !(e->ligne == t->ligne && e->pointsDeVie > 0 && e->position <= NB_EMPLACEMENTS + 1 && e->position > t->position)) {
            // on cherche l'ennemi à attaquer, sur la même ligne, vivant, devant la tourelle
            e = e->next;
        }
        if (e == NULL) {
            t = t->next;
            // pas d'ennemis à attaquer pour cette tourelle
            continue;
        }
        if (t->type == 'T') {
            // tourelle de type basique
            e->pointsDeVie -= 1;
        }
        if (t->type ==  'O' && t->position == e->position - 1) {
            // tourelle diplôme LSO mine
            e->pointsDeVie = 0;
            t->pointsDeVie = 0;
        }
        if (t->type == 'B') {
            // tourelle BU mur de défense
            // ne fait rien à l'ennemi
        }
        if (t->type == 'P') {
            // tourelle feuille de présence, immobilise l'ennemi pendant 2 tours
            if (e->immobilisation > 0) {
                continue;
            }
            if (e->tour%3 == 0) {
                e->immobilisation = 1;
            }
    
        }
        if (t->type == 'E') {
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

        if (e->pointsDeVie<=0) {
            if (e->type == 'S') {
                // S syndicaliste, augmente de 2 la vitesse des ennemis adjacents quand il meurt
                if (e->prev_line) {
                    e->prev_line->vitesse += 2;
                }
                if (e->next_line) {
                    e->next_line->vitesse += 2;
                }
            }
            SupprimerEnnemi(jeu, erreur, e);
        }
        t = t->next;
    }
    return;
}


void ResoudreActionsEnnemis(Jeu* jeu, Erreur* erreur) {
    Etudiant* e = jeu->etudiants;
    while (e != NULL) {
        if (e->pointsDeVie <= 0 || e->position > NB_EMPLACEMENTS + 1) {
            e = e->next;
            continue;
        }
        Tourelle* t = jeu->tourelles;
        if (t == NULL) {
            return;
        }

        if (e->type == 'D'){
            // parcourt tous les étudiants sur la même ligne ou une zone de 2 cases devant
            Etudiant* e2 = jeu->etudiants;
            while (e2 != NULL) {
                if ((e2->ligne >= e->ligne - 1 && e2->ligne <= e->ligne + 1) &&
                    (e2->position <= e->position + 3) &&
                    (e2->position >= e->position - 2) && (e2->position <= NB_EMPLACEMENTS)) {
                        // augment le points de vie de l'ennemi uniquement s'il n'est pas déjà au max
                    if (e2->pointsDeVie < trouverTypeEnnemi((char)e2->type)->pointsDeVie) {
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
        if (e->type == 'Z' || e->type == 'X' || e->type == 'S' || e->type == 'D') {
            // attaque de 1
            t->pointsDeVie -= 1;
        }
        if (e->type == 'L') {
            // plus gros dégâts, résistant mais très lent
            t->pointsDeVie -= 2;
        }
        if (e->type == 'F'){
            // fainéant, fait des sauts aléatoires ou ne bouge pas pendant plusieurs tours, attaque une fois sur deux
            int choix = rand() % 2;
            if (choix == 0) {
                t->pointsDeVie -= 1;
            }
        }
            
        e = e->next;
    }
    return;
}


void DeplacerEnnemis(Jeu* jeu, Erreur* erreur) {
    // déplace les ennemis en fonction de leur vitesse, prend en compte les collisions
    Etudiant* e = jeu->etudiants;
    while (e) {
        // deplace indique si le fainéant a déjà bougé pendant ce tour
        if (e->pointsDeVie <= 0 || e->position > NB_EMPLACEMENTS +1 || e->deplace == 1) {
            e = e->next;
            continue;
        }
        // action de la feuille de présence
        if (e->immobilisation > 0) {
            e->immobilisation--;
            e = e->next;
            continue;
        }
        int deplacement = e->vitesse;
        // on garde next au cas où le chainage change
        Etudiant * next = e->next;

        if (e->type == 'F') {
            // Fainéant : comportement aléatoire
            if (e->position >= NB_EMPLACEMENTS ) {
                // l'ennemi est sur la case d'apparition ou la dernière case
                deplacement = 1;
            }
            else {
                deplacement = ActionFaineant(jeu, e);
                if (deplacement == 0) {
                    e = next;
                    continue;
                }
            }
            e->deplace = 1;
        }

        // vérification si il y a une attaque de tourelle qui définit la vitesse (R)
        Tourelle* t1 = jeu->tourelles;
        while (t1) {
            // si pour cette tourelle l'ennemi est celui qu'elle devrait attaquer
            if (t1->ligne == e->ligne && t1->pointsDeVie > 0 && t1->position < e->position && e->prev_line == NULL) {
                if (t1->type == 'R') { // tourelle eduroam
                    if (e->position == NB_EMPLACEMENTS + 1) {
                        deplacement = 1;
                        t1 = t1->next;
                        continue;
                    }
                    int choix = rand() % 3;
                    switch (choix) {
                        case 0:
                            deplacement = 1;
                            break;
                        case 1:
                            deplacement = 0;
                            break;
                        case 2:
                            // on ne fait pas reculer l'ennemi s'il vient d'apparaître
                            if (e->position == NB_EMPLACEMENTS) {
                                deplacement = 0;
                                break;
                            }
                            deplacement = -1;
                            break;
                    }
                }
            }
            t1 = t1->next;
        }
        // vérification collision ennemi devant
        if (e->prev_line != NULL){
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
        // vérification collision tourelle
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
        e = next;
    }
}

int PartiePerdue(Jeu* jeu) {
    Etudiant* e = jeu->etudiants;
    while (e) {
        // si l'ennemi a atteint le bout de la ligne
        if(e->position <= 0) return 1;
        e->deplace = 0; // on réinitialise le compteur de déplacement
        e = e->next;
    }
    return 0;
}

int PartieGagnee(Jeu* jeu) {
    Etudiant* e = jeu->etudiants;
    while (e) {
        if (e->pointsDeVie > 0) {
            return 0; // il reste un ennemi en vie
        }
        e = e->next;
    }
    return 1;
}

void JouerPartie(Jeu* jeu, Erreur* erreur) {
    while(255) {
        jeu->tour++;

        ApparitionEnnemis(jeu, erreur);
        if(erreur->statut_erreur) return;

        // afficher le plateau en haut du terminal
        printf("\033[0;0H"); 
        printf("\033[2J");
        printf("\n              ========== DÉBUT DU TOUR %d ==========\n\n", jeu->tour);
        AfficherPlateau(jeu);
        printf(ANSI_TEXTE_GRIS " Appuyez sur Entrée pour continuer...\n" ANSI_RESET);
        printf("        Appuyez sur Entrée pour continuer ou tapez 'S' pour sauvegarder et quitter : ");
        char c = getchar();
        if (c == 'S' || c == 's') {
            while(getchar() != '\n'); 
            SauvegarderPartie(jeu, erreur);
            return;
        }
        while(getchar() != '\n'); 


        ResoudreActionsTourelles(jeu, erreur);
        if(erreur->statut_erreur) return;

        ResoudreActionsEnnemis(jeu, erreur);
        if(erreur->statut_erreur) return;

        DeplacerEnnemis(jeu, erreur);
        if(erreur->statut_erreur) return;

        if(PartiePerdue(jeu)) {
            printf("\033[0;0H"); 
            printf("\033[2J");
            jeu->score = 0;
            printf("         "ANSI_BG_VERT_FONCE ANSI_TEXTE_BLANC"  Fin de Partie   " ANSI_RESET "\n\n\n");
            AfficherPlateau(jeu);
            printAvecDelai("\n         "ANSI_BG_VERT_FONCE ANSI_TEXTE_BLANC"  Vous avez perdu... Les étudiants ont pris l'université.  "ANSI_RESET"\n", 50);
            while ((getchar()) != '\n');
            break;
        }

        if(PartieGagnee(jeu)) {
            printf("\033[0;0H"); 
            printf("\033[2J");
            jeu->score += jeu->cagnotte * 3;
            printf("         "ANSI_BG_BLEU_MEGA_LIGHT ANSI_TEXTE_BLEU_FONCE"    Fin de Partie   " ANSI_RESET "\n\n\n");
            AfficherPlateau(jeu);
            AjouterAuLeaderboard(jeu, erreur);
            if (erreur->statut_erreur) return;
            printAvecDelai("\n         "ANSI_BG_BLEU_MEGA_LIGHT ANSI_TEXTE_BLEU_FONCE"  Bravo, vous avez défendu l'université !  " ANSI_RESET "\n", 50);
            while ((getchar()) != '\n');
            
            // on récupère le nom du niveau
            char* nom_base = RecupererNom(jeu->fichier_ennemis, erreur);

            if (nom_base) {
                char cheminLeaderboard[MAX_NAME_LEN];
                snprintf(cheminLeaderboard, sizeof(cheminLeaderboard), "data_leaderboard/%s_leaderboard.txt", nom_base);
                strncpy(jeu->fichier_ennemis, cheminLeaderboard, sizeof(jeu->fichier_ennemis) - 1);
                jeu->fichier_ennemis[sizeof(jeu->fichier_ennemis) - 1] = '\0';
                AfficherLeaderboard(jeu->fichier_ennemis, erreur);
            }
            break;
        }
    }
}