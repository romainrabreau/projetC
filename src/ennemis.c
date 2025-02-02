#include "header.h"


const TypeEnnemi TYPES_ENNEMIS[] = {
    // symbole, points de vie, vitesse, nom, description
    {'Z', 3, 2, "Étudiant", "Étudiant de base, avance à moitié endormi"},
    {'E', 8, 1, "Étudiant L1", "Résistant mais aussi le plus lent, fait de gros dégâts sur sa ligne"},
    {'X', 2, 4, "Étudiant Talent", "Plus rapide, moins résistant"},
    {'S', 3, 1, "Syndicaliste", "Augmente la vitesse des ennemis adjacents de sa ligne lorsqu'il meurt"},
    {'F', 6, 3, "Fainéant", "Fait des sauts aléatoires (parfois sur la ligne d'en dessous) ou ne bouge pas pendant plusieurs tours, résistant"},
    {'D', 6, 1, "Doctorant", "Résistant et soigne les ennemis de 1 PV par tour sur une zone de 3 cases et 3 lignes."}
};


// retourne le type d'ennemi correspondant au symbole
const TypeEnnemi* trouverTypeEnnemi(char symbole) {
    for(int i = 0; i < NB_TYPES_ENNEMIS; i++) {
        if(TYPES_ENNEMIS[i].symbole == symbole) {
            return &TYPES_ENNEMIS[i];
        }
    }
    return NULL;
}


// vérifie que le type est dans les types autorisés
const TypeEnnemi* VerifType(int *tour, int *ligne, char *symbole, Erreur *erreur) {
    const TypeEnnemi* type_ennemi = trouverTypeEnnemi(*symbole);
    if (type_ennemi == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "type d'ennemi invalide\n");
        return NULL;
    }
    return type_ennemi;
}


// on considère que chaque élement dans le fichier d'apparition d'un ennemi est unique et triée par ordre croissant
// tri d'abord par tour puis par ligne
Etudiant* InitialisationEnnemis(FILE* fichier_ennemis, Jeu* jeu, Erreur* erreur) {
    if (fichier_ennemis == NULL) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "le fichier n'a pas pu être ouvert\n");
        return NULL;
    }

    printf("    Plusieurs ennemis feront leur apparition au cours de ce jeu : \n\n");
    for (int i = 0; i < NB_TYPES_ENNEMIS; i++) {
        printf(ANSI_BG_VERT_FONCE ANSI_TEXTE_BLANC "        %c" ANSI_RESET ": " ANSI_TEXTE_BLEU_FONCE "%s\n"ANSI_RESET, TYPES_ENNEMIS[i].symbole, TYPES_ENNEMIS[i].nom);
        printf("            ⟡ Points de vie : " ANSI_TEXTE_BLEU_MOYEN "%d\n" ANSI_RESET, TYPES_ENNEMIS[i].pointsDeVie);
        printf("            ⚡︎ Vitesse : " ANSI_TEXTE_BLEU_MOYEN "%d \n" ANSI_RESET, TYPES_ENNEMIS[i].vitesse);
        printf("            ⊟ Description : " ANSI_TEXTE_BLEU_MOYEN "%s \n\n" ANSI_RESET, TYPES_ENNEMIS[i].description);
    }
    printf("\n");
    printf("        À vous de les arrêter !\n\n");



    Etudiant* premier = NULL;  // premier ajouté
    Etudiant* dernier = NULL;  // dernier ajouté
    Etudiant* lignes_ennemis[NB_LIGNES] = {NULL}; // tableau des derniers ajoutés par ligne

    char ligne_fichier[256];

    while (fgets(ligne_fichier, sizeof(ligne_fichier), fichier_ennemis)) {
        int tour, num_ligne;
        char symbole_type;
        // pour chaque ligne on veut avoir trois valeurs corrcetes
        if (sscanf(ligne_fichier, "%d %d %c", &tour, &num_ligne, &symbole_type) != 3) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "format de ligne invalide\n");
            return NULL;
        }
        // obtenir type d'ennemi à partir du symbole correspondant
        const TypeEnnemi* type = VerifType(&tour, &num_ligne, &symbole_type, erreur);
        if (erreur->statut_erreur == 1) {
            LibererEnnemis(premier);
            return NULL;
        }

        // espace mémoire alloué pour l'étudiant
        Etudiant* nouvel_etudiant = (Etudiant*)malloc(sizeof(Etudiant));
        if (nouvel_etudiant == NULL) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "erreur d'allocation mémoire (ennemi)\n");
            LibererEnnemis(premier);
            return NULL;
        }

        // num_ligne commence à 1
        nouvel_etudiant->ligne = num_ligne;
        nouvel_etudiant->tour = tour;
        nouvel_etudiant->position = NB_EMPLACEMENTS + 99;

        nouvel_etudiant->next = NULL;
        nouvel_etudiant->next_line = NULL;
        nouvel_etudiant->prev_line = NULL;
        
        nouvel_etudiant->vitesse = type->vitesse;
        nouvel_etudiant->immobilisation = 0;
        nouvel_etudiant->deplace = 0;

        nouvel_etudiant->type = (int)type->symbole;
        nouvel_etudiant->pointsDeVie = type->pointsDeVie;

        // ajout à la fin de la liste principale
        if (premier == NULL) {
            premier = nouvel_etudiant;
        } else {
            dernier->next = nouvel_etudiant;
        }
        dernier = nouvel_etudiant;

        // chaînage par ligne
        if (lignes_ennemis[num_ligne - 1] == NULL) {
            lignes_ennemis[num_ligne - 1] = nouvel_etudiant;
        } else {
            nouvel_etudiant->prev_line = lignes_ennemis[num_ligne - 1];
            lignes_ennemis[num_ligne - 1]->next_line = nouvel_etudiant;
            // on met à jour la référence au dernier ennemi de la ligne de jeu
            lignes_ennemis[num_ligne - 1] = nouvel_etudiant;
        }
    }

    return premier;
}



// libère dynamiquement la mémoire allouée pour les ennemis
void LibererEnnemis(Etudiant* premier) {
    while (premier != NULL) {
        Etudiant* courant = premier;
        premier = premier->next;
        free(courant);
    }
}

void SupprimerEnnemi(Jeu* jeu, Erreur* erreur, Etudiant* ennemi) {

    if (!ennemi || !jeu) {
        erreur->statut_erreur=1;
        strcpy(erreur->msg_erreur, "Erreur d'accès à la donnée de l'ennemi\n");
    }
    // comptabiliser ennemi supprimé pour le score
    AjouterAuScore(jeu, ennemi, erreur);

    // chainage
    if (ennemi == jeu->etudiants) {
        // premier de la liste
        jeu->etudiants = ennemi->next;
    } else {
        // on trouve l'ennemi précédent
        Etudiant* prec = jeu->etudiants;
        while (prec && prec->next != ennemi) {
            prec = prec->next;
        }
        if (!prec) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Impossible de supprimer l'ennemi : ennemi introuvable\n");
            return;
        }
        prec->next = ennemi->next;
    }

    // chaînage par ligne
    if (ennemi->prev_line) {
        ennemi->prev_line->next_line = ennemi->next_line;
    }
    if (ennemi->next_line) {
        ennemi->next_line->prev_line = ennemi->prev_line;
    }
    printf("L'ennemi a été supprimé\n");
    free(ennemi);
}

int ActionFaineant(Jeu* jeu, Etudiant* e) {
    int choix = rand() % 6; 
    if (choix == 0) {
        // 1 fois sur 3 se place derrière son prédecesseur
        printf("choix 0\n");
        if (e->prev_line) {
            e->position = e->prev_line->position + 1;
            return 0;
        }
        else return 3;
    } else if (choix == 1) {
        printf("choix 1\n");
        // 1 fois sur trois saute sur la ligne du dessus si elle est vide
        if (e->ligne < NB_LIGNES) {

            Etudiant* curseur = jeu->etudiants;
            while (curseur) {
                if (curseur->ligne == e->ligne && curseur->pointsDeVie > 0) {
                    break;
                }
                curseur = curseur->next;
            }
            if (!curseur) {
                // chainage direct de la ligne initiale
                if (e->prev_line) {
                    e->prev_line->next_line = e->next_line;
                }
                if (e->next_line) {
                    e->next_line->prev_line = e->prev_line;
                }
                // on place l'ennemi
                e->prev_line = NULL;
                e->next_line = NULL;
                
                e->ligne= e->ligne + 1;
                return 3;
            } 
            if (curseur->position>=e->position) {
                printf("les ennemis sont trop loin\n");
                printf("position de l'ennemi derrière : %d\n", curseur->position);
                e->next_line = curseur;
                curseur->prev_line = e;
                return 3;
            }
            return 3;
            
        }
        if (e->ligne == NB_LIGNES) {
            // se place derrière son prédecesseur
            if (e->prev_line) {
                e->position = e->prev_line->position + 1;
                return 0;
            }
            return 3;
        }
        return 3;
    } else {
        // ne fait rien (2/3)
        printf("Le Fainéant reste immobile ce tour.\n");
        return 0;
    }
}