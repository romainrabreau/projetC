#include "header.h"

const TypeTourelle TYPES_TOURELLES[] = {
    // symbole, points de vie, prix, nom
    {'T', 3, 100, "Tableau noir"},      // Tourelle de base
    {'L', 1, 100, "Diplôme LSO"},       // Mine qui explose au contact de n'importe quel ennemi.
    {'B', 10, 200, "BU"},               // Tourelle de type mur
    {'E', 2, 150, "Emmanuel Lazard"},   // dégats sur une zone de 3 lignes et 3 positions devant elle
    {'R', 1, 75, "Eduroam"},            // Ralentit un ennemi (vitesse = 1)
    {'F', 2, 300, "Feuille de présence"}, 
};


const TypeTourelle* trouverTypeTourelle(char symbole) {
    for (int i = 0; i < NB_TYPES_TOURELLES; i++) {
        if (TYPES_TOURELLES[i].symbole == symbole) {
            return &TYPES_TOURELLES[i];
        }
    }
    return NULL;
}

Tourelle * InitialisationTourelles(int * cagnotte, Erreur* erreur){

    printf("Voici les tourelles disponibles ainsi que leurs caractéristiques :\n\n");
    for (int i = 0; i < NB_TYPES_TOURELLES; i++) {
        printf("%c : %s\n", TYPES_TOURELLES[i].symbole, TYPES_TOURELLES[i].nom);
        printf("    Points de vie : %d\n", TYPES_TOURELLES[i].pointsDeVie);
        printf("    Prix : %d\n\n", TYPES_TOURELLES[i].prix);
    }
    printf("\n");
        //TODO si on ajoute des tourelles spéciales
    printf("Certaines tourelles ont des caractéristiques spéciales, notamment : \n\n");
    
    printf("Il vous faut maintenant placer les tourellles de défense sur les emplacements de votre choix.\n");
    printf("Vous avez à défendre %d lignes, avec %d positions par ligne \n\n", NB_LIGNES, NB_EMPLACEMENTS);

    printf("Si vous souhaitez placer des tourelles sur la ligne proposée, entrez [SYMBOLE_X] [EMPLACEMENT_1], [SYMBOLE_Y] [EMPLACEMENT_2], ...\n");

    printf("SYMBOLE_X doit être un symbole de tourelle valide, et EMPLACEMENT_i doit être un entier entre 1 et %d\n", NB_EMPLACEMENTS);

    printf("Par exemple : A 3, A 5, A 12\n\n");

    printf("Sinon, faites entrée pour passer à la ligne suivante\n\n");
    printf("Vous ne pouvez pas placer deux tourelles sur le même emplacement\n\n");
    printf("Vous ne pouvez pas dépenser plus que votre cagnotte.\n");

    Tourelle* premier = NULL;
    Tourelle* dernier = NULL;
    int cout_total;

    for (int i = 1; i <= NB_LIGNES; i++) {
        printf("\n\n");
        printf("Il vous reste %d ECTS à dépenser.\n", *cagnotte);
        printf("Ligne %d : ", i);
        char ligne_tourelles[256];

        fgets(ligne_tourelles, sizeof(ligne_tourelles), stdin);

        if (ligne_tourelles[0] == '\n') {
            continue;
        }

        cout_total = VerifEntreeLigne(ligne_tourelles, erreur);
        if (cout_total == -1) {
            printf("%s", erreur->msg_erreur);
            erreur->statut_erreur=0;
            printf("Veuillez re-placer les tourelles en respectant le format et les symboles\n");
            i--;
            continue;
        }
        if (cout_total > *cagnotte) {
            printf("Vous avez dépassé le solde de votre cagnotte de %d ECTS, replacez s.v.p.\n", cout_total - *cagnotte);
            i--;
            continue;
        }
        else {
            printf("Vous avez dépensé %d ECTS pour cette ligne\n", cout_total);
            *cagnotte -= cout_total;
            //met à jour
            if (premier == NULL) {
                premier = AjouterTourelles(premier, dernier, ligne_tourelles, i, erreur);
                dernier = premier;
            }
            else {
                dernier = AjouterTourelles(premier, dernier, ligne_tourelles, i, erreur);
            }

            //TODO
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
    2. La validité du numéro de la position (entre 0 et NB_EMPLACEMENTS-1)
    3. Pas de doublons dans les positions
    4. Les paires sont symbole, position
    5. Pas de dépassement de la cagnotte

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
    
    // paire symbole position
    ////TODO possible, renvoyer l'erreur exacte
    //if (sscanf(ptr, " %c %d", &symbole, &position) != 2) {
        //erreur->statut_erreur = 1;
        //strcpy(erreur->msg_erreur, "La première entrée est invalide\n");
        //return -1;
    //}

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
Tourelle* AjouterTourelles(Tourelle* premier, Tourelle* dernier, char* ligne_tourelles, int ligne, Erreur* erreur) {
    char symbole;
    int position;
    int nb_matchs;
    // pointeur vers la paire symbole position
    char* ptr = ligne_tourelles;
    // on trouve la dernière tourelle pour y juxtaposer les nouvelles

    while ((nb_matchs = sscanf(ptr, " %c %d", &symbole, &position)) == 2) {

        Tourelle* nouvelle_tourelle = (Tourelle*)malloc(sizeof(Tourelle));
        if (nouvelle_tourelle == NULL) {
            erreur->statut_erreur = 1;
            printf("Erreur d'allocation mémoire\n");
            printf("Les tourelles ajoutées vont être supprimées\n");
            strcpy(erreur->msg_erreur, "Erreur d'allocation mémoire");
            LibererTourelles(premier);
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
        if (premier == NULL) {
            premier = nouvelle_tourelle;
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
    while (premier != NULL) {
        Tourelle* temp = premier;
        premier = premier->next;
        free(temp);
    }
}