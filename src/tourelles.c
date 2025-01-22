#include "header.h"

// Types de Tourelles (symbole, pointsDeVie, degats, prix, nom)
const TypeTourelle TYPES_TOURELLES[] = {
    { 'A', 5, 1, 50, "Tableau noir" },
};

const TypeTourelle* trouverTypeTourelle(char symbole) {
    for (int i = 0; i < NB_TYPES_TOURELLES; i++) {
        if (TYPES_TOURELLES[i].symbole == symbole) {
            return &TYPES_TOURELLES[i];
        }
    }
    return NULL;
}

/*
 *  InitialiserTourelles
 *  - Affiche la liste des tourelles disponibles
 *  - Demande à l'utilisateur de positionner ses tourelles
 *  - Retourne la liste chaînée de tourelles créées
 */
Tourelle* InitialiserTourelles(int *cagnotte, Erreur* erreur) {
    if (!cagnotte || !erreur) {
        fprintf(stderr, "Paramètres invalides à InitialiserTourelles.\n");
        return NULL;
    }

    printf("Voici les tourelles disponibles ainsi que leurs caractéristiques :\n");
    for (int i = 0; i < NB_TYPES_TOURELLES; i++) {
        printf("%c : %s\n", TYPES_TOURELLES[i].symbole, TYPES_TOURELLES[i].nom);
        printf("Points de vie : %d\n",   TYPES_TOURELLES[i].pointsDeVie);
        printf("Dégâts : %d\n",         TYPES_TOURELLES[i].degats);
        printf("Prix : %d\n\n",         TYPES_TOURELLES[i].prix);
    }

    // Exemples d'éventuelles caractéristiques spéciales
    printf("Certaines tourelles peuvent avoir des caractéristiques spéciales...\n\n");

    printf("Vous avez %d ECTS pour défendre %d lignes,\n", *cagnotte, NB_LIGNES);
    printf("avec %d emplacements par ligne.\n", NB_EMPLACEMENTS);
    printf("Pour placer des tourelles, entrez :\n");
    printf("   [SYMBOLE_X] [EMPLACEMENT_1], [SYMBOLE_Y] [EMPLACEMENT_2], ...\n");
    printf("Ou appuyez sur Entrée pour passer à la ligne suivante.\n");
    printf("(SYMBOLE_X doit être un symbole de tourelle valide.\n");
    printf(" EMPLACEMENT_i doit être un entier [0..%d].)\n\n", NB_EMPLACEMENTS - 1);

    Tourelle* premier = NULL;  // Liste chaînée vide au départ

    for (int i = 0; i < NB_LIGNES; i++) {
        printf("Il vous reste %d ECTS.\n", *cagnotte);
        printf("Ligne %d : ", i);

        char ligne_tourelles[256];
        if (!fgets(ligne_tourelles, sizeof(ligne_tourelles), stdin)) {
            // Erreur ou EOF
            break;
        }
        if (ligne_tourelles[0] == '\n') {
            // L'utilisateur n'a rien saisi, on passe à la ligne suivante
            continue;
        }

        // On vérifie la validité de l'entrée et on récupère le coût total
        int cout_total = VerifEntreeLigne(ligne_tourelles);
        if (cout_total == -1) {
            printf("Entrée invalide. Veuillez re-placer les tourelles.\n\n");
            i--; // on refait la même ligne
            continue;
        }

        // Vérifie si on ne dépasse pas la cagnotte
        if (cout_total > *cagnotte) {
            printf("Vous dépassez votre cagnotte de %d ECTS, re-placer s.v.p.\n\n",
                   cout_total - *cagnotte);
            i--;
            continue;
        }

        // Sinon on ajoute les tourelles à la liste
        *cagnotte -= cout_total;
        premier = AjouterTourelles(premier, ligne_tourelles, i);
        // TODO : si besoin, on peut visualiser la ligne tout de suite
    }

    return premier;
}

/*
 *  VerifEntreeLigne
 *  - Analyse la chaîne saisie par l'utilisateur (ex: "A 2,B 3")
 *  - Pour chaque paire (symbole, position), vérifie :
 *      * symbole valide
 *      * position dans [0..NB_EMPLACEMENTS - 1]
 *      * pas de doublon de position
 *  - Retourne le coût total (prix) si tout est OK
 *  - Retourne -1 si format invalide
 */
int VerifEntreeLigne(char* ligne_tourelles) {
    if (!ligne_tourelles) return -1;

    int positions[NB_EMPLACEMENTS] = {0};  // Pour marquer les positions déjà utilisées
    int solde_courant = 0;
    char symbole;
    int position;
    char* ptr = ligne_tourelles;

    // On essaie de lire tant qu'on trouve "SYMBOLE [espace] POSITION"
    while (sscanf(ptr, " %c %d", &symbole, &position) == 2) {
        // Vérif symbole
        const TypeTourelle* type = trouverTypeTourelle(symbole);
        if (!type) {
            return -1;  // symbole inconnu
        }

        // Vérif position
        if (position < 0 || position >= NB_EMPLACEMENTS) {
            return -1;  // hors bornes
        }
        if (positions[position]) {
            return -1;  // déjà utilisée
        }
        positions[position] = 1;

        // Ajout du prix
        solde_courant += type->prix;

        // On avance ptr jusqu'à la virgule ou fin
        // en sautant les caractères (symboles et positions)
        while (*ptr && *ptr != ',' && *ptr != '\n') {
            ptr++;
        }
        if (*ptr == ',') {
            // on saute la virgule
            ptr++;
            // si virgule suivie d'une autre virgule => format invalide
            if (*ptr == ',') return -1;
        }
    }

    // Si on n'a rien lu dès le départ => -1
    // (c'est vous qui voyez si vous voulez renvoyer 0 ou -1)
    return solde_courant;
}

/*
 *  AjouterTourelles
 *  - Parse de nouveau la ligne (ex: "A 2,B 3") 
 *  - Alloue chaque nouvelle tourelle et l'insère en fin de liste 
 */
Tourelle* AjouterTourelles(Tourelle* premier, char* ligne_tourelles, int ligne) {
    if (!ligne_tourelles) return NULL;

    char symbole;
    int position;
    char* ptr = ligne_tourelles;

    // On trouve le dernier élément de la liste "premier"
    Tourelle* dernier = premier;
    while (dernier && dernier->next != NULL) {
        dernier = dernier->next;
    }

    // On lit chaque paire
    while (sscanf(ptr, " %c %d", &symbole, &position) == 2) {
        const TypeTourelle* type = trouverTypeTourelle(symbole);
        if (!type) {
            break;
        }

        // Nouvel élément
        Tourelle* nouvelle = (Tourelle*)malloc(sizeof(Tourelle));
        if (!nouvelle) {
            printf("Erreur d'allocation mémoire pour la tourelle.\n");
            LibererTourelles(premier);
            return NULL;
        }
        nouvelle->type = (int)symbole;
        nouvelle->pointsDeVie = type->pointsDeVie;
        nouvelle->position = position;
        nouvelle->prix = type->prix;
        // si vous voulez stocker les degats : (pas inclus dans la struct Tourelle)
        // nouvelle->degats = type->degats; // (si vous ajoutez un champ 'degats')
        nouvelle->ligne = ligne;
        nouvelle->next = NULL;

        // Insertion en fin de liste
        if (!premier) {
            // Si la liste était vide, premier pointe sur cette tourelle
            premier = nouvelle;
            dernier = nouvelle; 
        }
        else {
            // On ajoute à la suite de 'dernier'
            dernier->next = nouvelle;
            dernier = nouvelle;
        }

        // Avance ptr jusqu'à la prochaine virgule ou fin
        while (*ptr && *ptr != ',' && *ptr != '\n') {
            ptr++;
        }
        if (*ptr == ',') {
            ptr++;
        }
    }
    return premier;
}

void supprimerTourelle(Jeu *jeu, Tourelle *t)
{
    if (!jeu || !t) return;
    
    Tourelle *current = jeu->tourelles;
    Tourelle *prev    = NULL;

    // Parcours de la liste pour trouver `t`
    while (current) {
        if (current == t) {
            // Si c'est le premier élément
            if (prev == NULL) {
                // On déplace le head de la liste
                jeu->tourelles = current->next;
            }
            else {
                // Sinon on relie le précédent à l'élément suivant
                prev->next = current->next;
            }
            
            // On libère la mémoire de la tourelle
            free(current);
            return;  // important : on sort après la suppression
        }
        prev    = current;
        current = current->next;
    }
    
    // Si la tourelle n'a pas été trouvée, on ne fait rien de plus
}


/*
 *  LibererTourelles
 *  - Parcourt la liste chaînée et libère chaque élément.
 */
void LibererTourelles(Tourelle* premier) {
    while (premier) {
        Tourelle* temp = premier;
        premier = premier->next;
        free(temp);
    }
}
