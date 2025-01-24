#include "header.h"

void ApparitionEnnemis(Jeu* jeu, Erreur* erreur) {
    Etudiant* e = jeu->etudiants;
    while (e != NULL) {
        if (e->tour == jeu->tour) {
            e->position = NB_EMPLACEMENTS;
        }
        e = e->next;
    }
    return;
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
        while (!(e->ligne == t->ligne && e->pointsDeVie > 0 && e->position <= NB_EMPLACEMENTS && e->position > t->position)) {
            // si l'ennemi est mort, ou derrière la tourelle, ounsurune ligne différente
                e = e->next;
        }
        if (e == NULL) {
            // pas d'ennemis à attaquer pour cette tourelle
            continue;
        }
        if ((char)t->type == 'A') {
            // tourelle de type basique
            e->pointsDeVie -= 2;
            printf("Tourelle de type basique a attaqué un étudiant sur la ligne %d à l'emplacement %d\n", e->ligne, e->position);
        }
        //TODO si tourelles spéciales implémenter
        t = t->next;
    }
    return;
}

void ResoudreActionsEnnemis(Jeu* jeu, Erreur* erreur) {
    Etudiant* e = jeu->etudiants;
    while (e != NULL) {
        if (e->pointsDeVie <= 0 || e->position<0) {
            e = e->next;
            continue;
        }
        Tourelle* t = jeu->tourelles;
        if (t == NULL) {
            return;
        }
        // on trouve la tourelle que va attaquer l'ennemi, pour les ennemis qui attaquent celle devant eux
        while (!(t->ligne == e->ligne && t->pointsDeVie > 0 && e->position == t->position + 1)) {
            // si la tourelle est morte, ou pas direct devant l'ennemi, ou sur une ligne différente
            t = t->next;
        }
        if (t == NULL) {
            e = e->next;
            continue; 
        }
        //TODO si ennemis spéciaux ajoutés, sinon aucun dégât de leur part
        printf("Action de l'ennemi de type %c sur la ligne %d à la position %d\n", (char)e->type, e->ligne, e->position);
        if ((char)e->type == 'Z') {
            // étudiant basique
            t->pointsDeVie -= 1;
            printf("étudian Z a attaqué une tourelle sur la ligne %d à l'emplacement %d\n", t->ligne, t->position);
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
        if (e->pointsDeVie <= 0) {
            e = e->next;
            continue;
        }
        int deplacement = e->vitesse;
        if (e->prev_line != NULL){
            // plus on est loin, plus la position est grande
            int diff = (e->position + e->vitesse) - e->prev_line->position ;
            // si l'ennemi est trop proche de celui de devant
            if (diff <= 0 ){
                deplacement = e->position - (e->prev_line->position + 1);
            }
        }
        // on doit vérifier si une tourelle ne bloque pas le passage
        Tourelle* t = jeu->tourelles;
        while (t) {
            // vérifie si la tourelle est sur la même ligne, si elle est vivante, si elle est devant l'ennemi et si elle est sur la trajectoire
            if (t->ligne == e->ligne && t->pointsDeVie > 0 && t->position < e->position && t->position > e->position + deplacement) {
                int deplacement_possible = e->position - t->position - 1;
                if (deplacement_possible < deplacement) {
                    // on garde le pire cas
                    deplacement = deplacement_possible;
                }
            }
            t = t->next;
        }
        printf("Déplacement de l'ennemi de type %c sur la ligne %d de %d cases \n", (char)e->type, e->ligne, deplacement);
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
    while(18) {
        jeu->tour++;
        printf("Tour %d\n", jeu->tour);

        ApparitionEnnemis(jeu, err);
        if(err->statut_erreur) return;

        ResoudreActionsTourelles(jeu, err);
        if(err->statut_erreur) return;

        ResoudreActionsEnnemis(jeu, err);
        if(err->statut_erreur) return;

        DeplacerEnnemis(jeu, err);
        if(err->statut_erreur) return;

        if(PartiePerdue(jeu)) {
            printf("Vous avez perdu... Les étudiants ont pris l'université.\n");
            break;
        }

        if(PartieGagnee(jeu)) {
            printf("Bravo, vous avez défendu l'université !\n");
            break;
        }

        AfficherPlateau(jeu);

        while ((getchar()) != '\n');
        printf("Appuyez sur Entrée pour continuer...\n");
        getchar();
    }
}