#include "header.h"


const TypeTourelle TYPES_TOURELLES[] = {
    {'A', 5, 10, "Archer"},      // Type de base
};

Tourelle * InitialiserTourelle(Erreur* erreur){
    

}

void LibererTourelles(Tourelle* premier) {
    while (premier != NULL) {
        Tourelle* temp = premier;
        premier = premier->next;
        free(temp);
    }
}
