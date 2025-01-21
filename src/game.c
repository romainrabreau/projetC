#include "header.h"

void ApparitionEnnemis(Jeu* jeu, Erreur* erreur) {
    Etudiant* e = jeu->etudiants;
    while (e != NULL) {
        if (e->tour == jeu->tour) {
            e->position = 0;
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
        while (!(e->ligne == t->ligne && e->pointsDeVie > 0 && e->position >= 0 && e->position > t->position)) {
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
            print("Tourelle de type basique a attaqué un étudiant sur la ligne %d à l'emplacement %d\n", e->ligne, e->position);
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
        // on trouve la tourelle que va attaquer l'ennemi, pour les ennemis qui attaquent devant
        while (!(t->ligne == e->ligne && t->pointsDeVie > 0 && t->position < e->position)) {
            // si la tourelle est morte, ou derrière l'ennemi, ou sur une ligne différente
            t = t->next;
        }
        if (t == NULL) {
            e = e->next;
            continue; 
        }
        //TODO si ennemis spéciaux ajoutés, sinon aucun dégât de leur part
        printf("Action de l'ennemi de type %c sur la ligne %d à la position %d\n", (char)e->type, e->ligne, e->position);
        e = e->next;
    }
    return;
}


void DeplacerEnnemis(Jeu* jeu, Erreur* erreur) {
    // déplace les ennemis en fonction de leur vitesse, prend en compte les collisions
    Etudiant* e = jeu->etudiants;
    while (e != NULL) {
        // si l'ennemi est mort
        if (e->pointsDeVie <= 0) {
            e = e->next;
            continue;
        }
        if (e->prev_line != NULL){
            int diff = e->prev_line->position - (e->position + e->vitesse);
            // si l'ennemi est trop proche de celui de devant
            if (diff <= 0 ){
                e->position = e->prev_line->position - 1;
                e = e->next;
                continue;
            }
        }
        e->position += e->vitesse;
        e = e->next;
    }
}

int PartiePerdue(Jeu* jeu) {
    Etudiant* e = jeu->etudiants;
    while (e != NULL) {
        // si l'ennemi a atteint le bout de la ligne
        if (e->position >= NB_EMPLACEMENTS) {
            return 1; // défaite
        }
        e = e->next;
    }
    return 0;
}

int PartieGagnee(Jeu* jeu) {
    Etudiant* e = jeu->etudiants;
    while (e != NULL) {
        if (e->pointsDeVie > 0) {
            return 0; // il reste un enneli en vie
        }
        e = e->next;
    }
    return 1; // victoire
}


// ----------------------------------------------------------------------------
// 8) (Optionnel) VisualiserEnnemis ou VisualiserJeu : affichage de la grille en ASCII
//    Tu peux séparer ça dans un interface.c, ou le laisser ici si tu préfères
// ----------------------------------------------------------------------------
void VisualiserEnnemis(Etudiant* liste, Erreur* erreur) {
    // Ex. simple : afficher juste la liste d’ennemis
    // Dans un vrai tower defense, tu peux faire un affichage 2D.
    printf("\033[0;0H"); 
    printf("\033[2J");

    printf("Vagues : \n");
    Etudiant* e = liste;
    while (e != NULL) {
        printf("Ennemi type '%c' => Tour: %d, Ligne: %d, Pos: %d, PV: %d\n",
               (char)(e->type), e->tour, e->ligne, e->position, e->pointsDeVie);
        e = e->next;
    }
    // Si tu veux manipuler l'erreur en cas de soucis, tu peux faire :
    // erreur->statut_erreur = 1;
    // strcpy(erreur->msg_erreur, "Blabla...");
}

