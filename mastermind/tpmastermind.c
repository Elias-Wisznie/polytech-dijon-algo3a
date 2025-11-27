#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum couleur { VIDE, JAUNE, BLEU, ROUGE, VERT, BLANC, MAUVE };
enum resultat { NON_VERIFIE, FAUX, PARTIEL, CORRECT };

typedef struct {
    enum couleur proposition[4];
    enum resultat evaluation[4];
} COMBINAISON;

typedef struct {
    int tour_courant;
    COMBINAISON code_secret;
    COMBINAISON historique[12];
} ETAT_JEU;

// Convertit une couleur enum en caractère à afficher
char couleur_en_char(enum couleur c) {
    switch (c) {
    case JAUNE: return 'Y';
    case BLEU: return 'B';
    case ROUGE: return 'R';
    case VERT: return 'G';
    case BLANC: return 'W';
    case MAUVE: return 'P';
    default: return ' ';
    }
}

// Convertit un caractère en couleur enum
enum couleur char_en_couleur(char c) {
    switch (c) {
    case 'Y': return JAUNE;
    case 'B': return BLEU;
    case 'R': return ROUGE;
    case 'G': return VERT;
    case 'W': return BLANC;
    case 'P': return MAUVE;
    default: return VIDE;
    }
}

// Convertit un résultat enum en caractère
char resultat_en_char(enum resultat r) {
    switch (r) {
    case NON_VERIFIE: return '?';
    case FAUX: return 'X';
    case PARTIEL: return '~';
    case CORRECT: return 'V';
    }
    return '?';
}

// Affiche une combinaison avec résultats partiel/correct
void afficher_combinaison(COMBINAISON* combo) {
    int nb_partiels = 0, nb_corrects = 0;
    for (int i = 0; i < 4; i++) {
        if (combo->evaluation[i] == PARTIEL) nb_partiels++;
        else if (combo->evaluation[i] == CORRECT) nb_corrects++;
        printf("%c ", couleur_en_char(combo->proposition[i]));
    }
    printf("| Partiel : %d | Correct : %d\n", nb_partiels, nb_corrects);
}

// Valide et convertit la saisie utilisateur en combinaison
int saisie_en_combinaison(COMBINAISON* combo, char* saisie) {
    if (strlen(saisie) != 4) return 1; // erreur longueur
    for (int i = 0; i < 4; i++) {
        combo->proposition[i] = char_en_couleur(saisie[i]);
        if (combo->proposition[i] == VIDE) return 1; // couleur invalide
        combo->evaluation[i] = NON_VERIFIE;
    }
    return 0;
}

// Génère aléatoirement le code secret
void generer_code_secret(COMBINAISON* combo) {
    char couleurs_possibles[] = { 'Y', 'B', 'R', 'G', 'W', 'P' };
    srand(time(NULL));
    for (int i = 0; i < 4; i++) {
        int r = rand() % 6;
        combo->proposition[i] = char_en_couleur(couleurs_possibles[r]);
        combo->evaluation[i] = NON_VERIFIE;
    }
}

// Affiche l'état complet du jeu (toutes les propositions)
void afficher_etat_jeu(ETAT_JEU* etat) {
    printf("Tour %d\n", etat->tour_courant);
    for (int i = 0; i < etat->tour_courant; i++) {
        afficher_combinaison(&etat->historique[i]);
    }
}

// Traite un tour : saisie, validation, comparaison, mise à jour état
int jouer_tour(ETAT_JEU* etat) {
    int indice = etat->tour_courant;
    char saisie[10];
    do {
        printf("Entrez 4 couleurs (Y B R G W P) : ");
        scanf("%9s", saisie);
    } while (saisie_en_combinaison(&etat->historique[indice], saisie) != 0);

    // Vérification résultat partiel/correct
    for (int i = 0; i < 4; i++) {
        if (etat->historique[indice].proposition[i] == etat->code_secret.proposition[i]) {
            etat->historique[indice].evaluation[i] = CORRECT;
        }
        else {
            int trouve = 0;
            for (int j = 0; j < 4; j++) {
                if (etat->historique[indice].proposition[i] == etat->code_secret.proposition[j]) {
                    trouve = 1;
                    break;
                }
            }
            etat->historique[indice].evaluation[i] = trouve ? PARTIEL : FAUX;
        }
    }

    etat->tour_courant++;
    afficher_etat_jeu(etat);

    int nb_corrects = 0;
    for (int i = 0; i < 4; i++) {
        if (etat->historique[indice].evaluation[i] == CORRECT) nb_corrects++;
    }
    return nb_corrects;
}

int main() {
    ETAT_JEU* etat = malloc(sizeof(ETAT_JEU));
    if (!etat) return 1;

    etat->tour_courant = 0;
    generer_code_secret(&etat->code_secret);

    // Initialisation de l'historique
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 4; j++) {
            etat->historique[i].proposition[j] = VIDE;
            etat->historique[i].evaluation[j] = NON_VERIFIE;
        }
    }

    int gagne = 0;
    while (etat->tour_courant < 12 && !gagne) {
        gagne = (jouer_tour(etat) == 4);
        if (gagne) {
            printf("Félicitations ! Vous avez réussi en %d tours.\n", etat->tour_courant);
        }
    }
    if (!gagne) {
        printf("Vous avez perdu, le code secret était : ");
        for (int i = 0; i < 4; i++) {
            printf("%c", couleur_en_char(etat->code_secret.proposition[i]));
        }
        printf("\n");
    }

    free(etat);
    return 0;
}
