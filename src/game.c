#include "header.h"

// Apparition de nouveaux ennemis (ex. position initiale, etc.)
void ApparitionEnnemis(Jeu* jeu, Erreur* err) {
    // Exemple simplifié :
    (void)err; // si vous n'utilisez pas err ici

    Etudiant* e = jeu->etudiants;
    while(e) {
        if(e->tour == jeu->tour && e->position >= NB_EMPLACEMENTS) {
            e->position = NB_EMPLACEMENTS; 
        }
        e = e->next;
    }
}

// Résolution des actions des tourelles
void ResoudreActionsTourelles(Jeu* jeu, Erreur* err) {
    (void)err; // ignorer le paramètre si non utilisé
    Tourelle* t = jeu->tourelles;
    while(t) {
        // On récupère le type de la tourelle pour connaître ses dégâts
        const TypeTourelle* tType = trouverTypeTourelle((char)t->type);
        if(t->pointsDeVie <= 0) {
            // Si la tourelle est détruite, on la supprime
            Tourelle* tmp = t->next;
            supprimerTourelle(jeu, t);
            t = tmp;
            continue;
        }
        // Tir sur un ennemi dans la même ligne (ex. un seul ennemi, le premier trouvé)
        Etudiant* e = jeu->etudiants;
        while(e) {
            Etudiant* e_next = e->next;
            if(e->ligne == t->ligne && e->pointsDeVie > 0) {
                // On inflige des dégâts
                e->pointsDeVie -= tType->degats;
                if(e->pointsDeVie <= 0) {
                    supprimerEtudiant(jeu, e);
                }
                break; // si la tourelle n'attaque qu'un ennemi par tour
            }
            e = e_next;
        }
        t = t->next;
    }
}

// Résolution des actions des ennemis
void ResoudreActionsEnnemis(Jeu* jeu, Erreur* err) {
    (void)err;
    Tourelle* t = jeu->tourelles;
    while(t) {
        Tourelle* t_next = t->next;
        if(t->pointsDeVie <= 0) {
            t = t_next;
            continue;
        }
        // Les ennemis sur la même ligne qui se trouvent juste devant la tourelle (pos = tourelle->pos - 1) attaquent
        Etudiant* e = jeu->etudiants;
        while(e) {
            Etudiant* e_next = e->next;
            if(e->ligne == t->ligne && e->position == t->position - 1 && e->pointsDeVie > 0) {
                const TypeEnnemi* eType = trouverTypeEnnemi((char)e->type);
                if(eType) {
                    t->pointsDeVie -= eType->degats;
                    if(t->pointsDeVie <= 0) {
                        supprimerTourelle(jeu, t);
                        break;
                    }
                }
            }
            e = e_next;
        }
        t = t_next;
    }
}

// Déplacement des ennemis
void DeplacerEnnemis(Jeu* jeu, Erreur* err) {
    (void)err;
    Etudiant* e = jeu->etudiants;
    while(e) {
        const TypeEnnemi* eType = trouverTypeEnnemi((char)e->type);
        if(!eType) {
            e = e->next;
            continue;
        }
        int vitesse = eType->vitesse;
        int newPos = e->position - vitesse;
        if(newPos < 0) {
            newPos = -1;
        }
        // Vérifie si une tourelle occupe newPos
        int blocked = 0;
        Tourelle* t = jeu->tourelles;
        while(t) {
            if(t->ligne == e->ligne && t->pointsDeVie > 0 && t->position == newPos) {
                blocked = 1;
                break;
            }
            t = t->next;
        }
        if(!blocked) {
            e->position = newPos;
        }
        e = e->next;
    }
}

// Vérifier défaite : si un ennemi atteint position < 0
int VerifierDefaite(Jeu* jeu) {
    Etudiant* e = jeu->etudiants;
    while(e) {
        if(e->position < 0) return 1;
        e = e->next;
    }
    return 0;
}

// Vérifier victoire : si tous les ennemis sont morts
int VerifierVictoire(Jeu* jeu) {
    Etudiant* e = jeu->etudiants;
    while(e) {
        if(e->pointsDeVie > 0) return 0; 
        e = e->next;
    }
    return 1;
}

// La boucle de jeu
void JouerPartie(Jeu* jeu, Erreur* err) {
    while(1) {
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

        if(VerifierDefaite(jeu)) {
            printf("Vous avez perdu... Les étudiants ont pris l'université.\n");
            break;
        }

        if(VerifierVictoire(jeu)) {
            printf("Bravo, vous avez défendu l'université !\n");
            break;
        }

        AfficherPlateau(jeu);

        printf("Appuyez sur Entrée pour continuer...\n");
        while(getchar() != '\n');
    }
}
