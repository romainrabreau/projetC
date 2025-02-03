#include "header.h"


const TypeTourelle TYPES_TOURELLES[] = {
    // symbole, points de vie, prix, nom, description
    {'T', 3, 80, "Tableau noir", "Tourelle de base, envoie des craies sur les étudiants, fait un dégat de 1"},
    {'O', 1, 40, "Diplôme LSO", "Tourrelle mine, explose au contact et détruit l'étudiant puis s'auto-détruit"},
    {'B', 10, 120, "BU", "Maxi mur de livres, ralentit les étudiants"},
    {'P', 2, 200, "Feuille de présence", "Immobilise l'étudiant pendant 2 tours"},
    {'E', 2, 200, "Emmanuel Lazard", "Dégâts de zone sur 3 lignes en même temps et 3 cases devant"},
    {'R', 1, 160, "Eduroam", "Comportement aléatoire, une fois sur trois, l'ennemi recule, avance de 1, ou avance de 2"},
};

    // tableau ASCII Art de l'ordinateur
const char* ordinateur[] = {
        "     .-----------------. ",
        "     |  >_ root@uni    | ",
        "     |  $ sudo hack    | ",
        "     |  [############] | ",
        "     |_________________| ",
        "    /                   \\ ",
        "   /_____________________\\ "
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
   InitialiserTourelles
   - Affiche la liste des tourelles disponibles
   - Demande à l'utilisateur de positionner ses tourelles
   - Retourne la liste chaînée de tourelles créées
 */
Tourelle * InitialisationTourelles(int * cagnotte, Erreur* erreur){
    // on stocke le tableau d'affichage des caractéristiques, on a 4 lignes par tourelle
    char lignesTourelles[NB_TYPES_TOURELLES * 4][256];

    int indexLigne = 0;
    // on remplit le tableau de lignes avec les informations 
    for (int i = 0; i < NB_TYPES_TOURELLES; i++) {
        sprintf(
            // indexLigne++ incrémente post-affectation
            lignesTourelles[indexLigne++],
            ANSI_BG_BLEU_MEGA_LIGHT "%c" ANSI_RESET " : "
            ANSI_TEXTE_BLEU_FONCE "%s" ANSI_RESET,
            TYPES_TOURELLES[i].symbole, TYPES_TOURELLES[i].nom
        );
        sprintf(
            lignesTourelles[indexLigne++],
            "    ⟡ Points de vie : "
            ANSI_TEXTE_BLEU_MOYEN "%d" ANSI_RESET,
            TYPES_TOURELLES[i].pointsDeVie
        );
        sprintf(
            lignesTourelles[indexLigne++],
            "    ∞ Prix : "
            ANSI_TEXTE_BLEU_MOYEN "%d ECTS" ANSI_RESET,
            TYPES_TOURELLES[i].prix
        );
        sprintf(
            lignesTourelles[indexLigne++],
            "    ⊟ Description : "
            ANSI_TEXTE_BLEU_MOYEN "%s" ANSI_RESET,
            TYPES_TOURELLES[i].description
        );
    }

    int nbLignesOrdinateur = 7;
    int nbLignesTourelles = indexLigne;
    int nbLignesMax = (nbLignesOrdinateur > nbLignesTourelles)
                      ? nbLignesOrdinateur : nbLignesTourelles;

    printf("\n\n");
    printf("    Voici les tourelles disponibles ainsi que leurs caractéristiques :\n\n\n");
    // affichage ASCII Art simultané avec les tourelles
    for (int l = 0; l < nbLignesMax; l++) {
        // affichage de la ligne ASCII Art ordinateur
        if (l < nbLignesOrdinateur) {
            printf("%-35s", ordinateur[l]); // largeur fixe pour aligner
        } else {
            printf("%-35s", "");
        }
        if (l < nbLignesTourelles) {
            printf("%s", lignesTourelles[l]);
        }

        printf("\n");
    }

    printf(ANSI_TEXTE_GRIS "Appuyez sur Entrée pour continuer...\n" ANSI_RESET);
    while (getchar() != '\n');
    printf("\n    𓆉  Il vous faut maintenant placer les tourellles de défense sur les emplacements de votre choix.\n");
    printf("        Vous avez à défendre %d lignes, avec %d positions par ligne \n\n", NB_LIGNES, NB_EMPLACEMENTS);

    printf("        Si vous souhaitez placer des tourelles sur la ligne proposée, entrez [SYMBOLE_X] [EMPLACEMENT_1], [SYMBOLE_Y] [EMPLACEMENT_2], ...\n");

    printf("        "ANSI_TEXTE_BLEU_FONCE ANSI_BG_BLANC "SYMBOLE_X" ANSI_RESET " doit être un symbole de tourelle valide, et " ANSI_TEXTE_BLEU_FONCE ANSI_BG_BLANC "EMPLACEMENT_i" ANSI_RESET " doit être un entier entre 1 et %d\n", NB_EMPLACEMENTS);

    printf("            Par exemple : " ANSI_TEXTE_BLANC ANSI_BG_BLEU_SHINY "➩ T 3, T 5, T 12" ANSI_RESET "\n\n");

    printf("        Sinon, faites "ANSI_TEXTE_BLEU_MOYEN "entrée" ANSI_RESET" pour passer à la ligne suivante\n\n");
    printf("    × Vous ne pouvez pas placer deux tourelles sur le même emplacement\n\n");
    printf("    × Vous ne pouvez pas dépenser plus que votre cagnotte.\n");

    Tourelle* premier = NULL;
    Tourelle* dernier = NULL;

    int cout_total;

    for (int i = 1; i <= NB_LIGNES; i++) {
        printf("\n\n");
        printf("    Il vous reste " ANSI_BG_BLEU_MEGA_LIGHT ANSI_TEXTE_BLEU_FONCE "%d ECTS" ANSI_RESET" à dépenser.\n", *cagnotte);
        printf("\n");
        printf(ANSI_TEXTE_BLANC ANSI_BG_BLEU_FONCE "    Ligne %d :" ANSI_RESET" ", i);
        char ligne_tourelles[256];

        fgets(ligne_tourelles, sizeof(ligne_tourelles), stdin);
        // si aucune tourelle n'est entrée
        if (ligne_tourelles[0] == '\n') {
            continue;
        }
        // vérifie la conformité de l'entrée utilisateur
        cout_total = VerifEntreeLigne(ligne_tourelles, erreur);
        if (cout_total == -1) {
            printf(ANSI_TEXTE_BLEU_MOYEN "   %s " ANSI_RESET, erreur->msg_erreur);
            printf(ANSI_RESET);
            erreur->statut_erreur=0;
            printf(ANSI_BG_ROUGE ANSI_TEXTE_BLANC"  × Veuillez re-placer les tourelles en respectant le format et les symboles ×   " ANSI_RESET);
            i--;
            continue;
        }
        if (cout_total > *cagnotte) {
            printf("Vous avez dépassé le solde de votre cagnotte de " ANSI_BG_BLEU_MEGA_LIGHT "%d ECTS" ANSI_RESET", replacez s.v.p.\n", cout_total - *cagnotte);
            i--;
            continue;
        }
        else {
            printf("Vous avez dépensé " ANSI_BG_BLEU_MEGA_LIGHT "%d ECTS" ANSI_RESET" pour cette ligne\n", cout_total);
            *cagnotte -= cout_total;
            //met à jour le premier et le dernier de la liste chaînée
            dernier = AjouterTourelles(&premier, dernier, ligne_tourelles, i, erreur);
        }
        if (erreur->statut_erreur) {
            return NULL;
        }
    }
    return premier;
}


int VerifEntreeLigne(char * ligne_tourelles, Erreur* erreur) {
    /* vérifie 
    1. La validité des symboles de tourelles
    2. La validité du numéro de la position (entre 1 et NB_EMPLACEMENTS)
    3. Pas de doublons dans les positions
    4. Les paires sont (symbole, position)

    Retourne le nombre d'ECTS dépensés
    */
   if (!ligne_tourelles) {
         erreur->statut_erreur = 1;
         strcpy(erreur->msg_erreur, "l'entrée est vide\n");
         return -1;
   }
    int positions[NB_EMPLACEMENTS] = {0};  // Pour marquer les positions utilisées
    int solde_courant = 0;
    char symbole;
    int position;
    int nb_matchs;
    char* ptr = ligne_tourelles;
    

    while ((nb_matchs = sscanf(ptr, " %c %d", &symbole, &position)) == 2) {
        // validité du symbole
        const TypeTourelle* type = trouverTypeTourelle(symbole);
        if (!type) return -1;  
        
        // validité de la position
        if (position < 1 || position > NB_EMPLACEMENTS) {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Le numéro de l'emplacement entré est invalide\n");
            return -1;
        }
        if (positions[position-1]){
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Vous ne pouvez pas placer deux tourelles au même endroit\n");
            return -1;
        }
        // une tourelle a été placée sur cette position
        positions[position-1] = 1;
        
        // renouvellement du solde
        solde_courant += type->prix;
        
        // avance au prochain couple symbole-position
        ptr++; // on avance jusqu'à l'espace
        if (*ptr != ' ') {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Assurez-vous d'avoir un espace entre le symbole et le numéro de l'emplacement\n");
            return -1;
        }
        ptr++;
        // doit être un int
        if (*ptr > '9' || *ptr < '0') {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Le format de l'emplacement entré est invalide\n");
            return -1;
        }
        for (; *ptr <= '9' && *ptr >= '0'; ptr++);

        // si après le nombre il n'y a pas de virgule ou de retour à la ligne c'est invalide
        if (*ptr && *ptr != ',' && *ptr != '\n') {
            erreur->statut_erreur = 1;
            strcpy(erreur->msg_erreur, "Assurez-vous d'avoir une virgule ou un retour à la ligne après le numéro de l'emplacement\n");
            return -1;
        }
        if (*ptr == '\n') break;
        ptr++;
        for (; *ptr && *(ptr) == ' '; ptr++);
    }
    if (nb_matchs != 2) {
        erreur->statut_erreur = 1;
        strcpy(erreur->msg_erreur, "Assurez-vous d'avoir le même nombre de symboles et de numéros d'emplacement\n");
        return -1;
    }
    
    return solde_courant;
}
// AjouterTourelles parcourt à nouveau la ligne et chaîne chaque entrée
Tourelle* AjouterTourelles(Tourelle* * premier, Tourelle* dernier, char* ligne_tourelles, int ligne, Erreur* erreur) {
    char symbole;
    int position;
    int nb_matchs;
    // pointeur vers la paire symbole position courante
    char* ptr = ligne_tourelles;
    
    while ((nb_matchs = sscanf(ptr, " %c %d", &symbole, &position)) == 2) {
        Tourelle* nouvelle_tourelle = (Tourelle*)malloc(sizeof(Tourelle));

        if (nouvelle_tourelle == NULL) {
            erreur->statut_erreur = 1;
            printf(ANSI_BG_ROUGE ANSI_TEXTE_BLANC"      Erreur d'allocation mémoire\n");
            printf(ANSI_BG_ROUGE ANSI_TEXTE_BLANC"      Les tourelles ajoutées vont être supprimées\n");
            strcpy(erreur->msg_erreur, "Erreur d'allocation mémoire");
            LibererTourelles(*premier);
            return NULL;
        }
        const TypeTourelle * type = trouverTypeTourelle(symbole);
        nouvelle_tourelle->type = (int)symbole;
        nouvelle_tourelle->pointsDeVie = type->pointsDeVie;
        nouvelle_tourelle->prix = type->prix;

        nouvelle_tourelle->ligne = ligne;
        nouvelle_tourelle->position = position;
        
        // chainage par next
        nouvelle_tourelle->next = NULL;

        if (*premier == NULL) {
            *premier = nouvelle_tourelle;
        }
        else {
            dernier->next = nouvelle_tourelle;
        }
        dernier = nouvelle_tourelle;

        //  avance au prochain couple symbole-position, même s'il y a des espaces
        while (*ptr && *ptr != ',' && *ptr != '\n') ptr++;
        if (*ptr == ',') ptr++;
    } 
    return dernier;   
}


void LibererTourelles(Tourelle* premier) {
    Tourelle* courant;
    while (premier != NULL) {
        courant = premier;
        premier = premier->next;
        free(courant);
    }
}
