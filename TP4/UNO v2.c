/*
==============================================
WISZNIEWSKI Elias
Algo 3A
TP4 - Jeu de cartes UNO
24/11/2025
==============================================
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NBR_CARTES 48   // Nombre total de cartes (4 couleurs x 12 valeurs)
#define CARTES_PAR_JOUEUR 7 // Nombre de cartes distribuées à chaque joueur au départ

// Noms des couleurs et des valeurs des cartes pour l'affichage
const char* couleurs[] = { "Rouge", "Vert", "Bleu", "Jaune" };
// Valeurs 0 à 9 numériques,+2 (valeur 10), +4 (valeur 11)
const char* valeurs[] = { "0","1","2","3","4","5","6","7","8","9","+2","+4" };

// Tableau global représentant le deck complet de cartes
int deck[NBR_CARTES];
int indexPioche = 0;    // Index pour suivre la prochaine carte à piocher

// Mains des joueurs, tableau pour stocker les cartes en main et leur taille
int mainJoueur[50];
int tailleMainJoueur = 0;

int mainBot[50];
int tailleMainBot = 0;

int topCard;    // Carte actuellement sur le dessus du tas (au centre)

/*
  Chaque carte du jeu est représentée par un entier unique de 0 à 47

  La carte est codée selon la formule :
      code = couleur * 12 + valeur

  Ou :
    - couleur est un entier de 0 à 3, correspondant aux couleurs :
        0 = Rouge, 1 = Vert, 2 = Bleu, 3 = Jaune
    - valeur est un entier de 0 à 11, correspondant aux cartes numériques et aux cartes spéciales :
      - 0 à 9 : cartes numériques 0 à 9
      - 10 : carte spéciale +2
      - 11 : carte spéciale +4

  Exemple :
    - Rouge 0   : 0 * 12 + 0 = 0
    - Vert 3    : 1 * 12 + 3 = 15
    - Bleu +2   : 2 * 12 + 10 = 34
    - Jaune +4  : 3 * 12 + 11 = 47

  Pour extraire la couleur et la valeur d'une carte on utilise :
    couleur = code / 12 (division entière)
    valeur  = code % 12 (reste de la division)
*/


// Fonction pour récupérer la couleur d'une carte
int couleur(int code) {
    return code / 12;
}

// Fonction pour récupérer la valeur d'une carte
int valeur(int code) {
    return code % 12;
}

// Affiche une carte en distinguant bien les cartes +2 et +4
void afficher_carte(int code) {
    int val = valeur(code);
	printf("%s ", couleurs[couleur(code)]); // La fonction sort le code couleur, et l'associe à son nom
    if (val == 10)
        printf("+2");
    else if (val == 11)
        printf("+4");
    else
        printf("%s", valeurs[val]);
}

// Initialise le deck en ordre croissant
void initialiser_deck() {
    int i = 0;
    for (int c = 0; c < 4; c++) {
        for (int v = 0; v < 12; v++) {
            deck[i] = c * 12 + v;
            i++;
        }
    }
}

// Mélange le deck https://fr.wikipedia.org/wiki/M%C3%A9lange_de_Fisher-Yates
void melanger_deck() {
    for (int i = NBR_CARTES - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }
}

// Vérifie si une carte peut être jouée
int peut_jouer(int code, int top) {
    if (couleur(code) == couleur(top)) return 1;
    if (valeur(code) == valeur(top)) return 1;
    return 0;
}

// Pioche une carte
void piocher(int main[], int* tailleMain, int afficher) {
    if (indexPioche >= NBR_CARTES) {
        if (afficher) printf("La pioche est vide \n");
        return;
    }
    int carte = deck[indexPioche];
    indexPioche++;
    main[*tailleMain] = carte;
    (*tailleMain)++;
    if (afficher) {
        printf("Vous piochez : ");
        afficher_carte(carte);
        printf("\n");
    }
}

// Enlève une carte
void enlever_carte(int main[], int* tailleMain, int k) { // Indice k, carte à retire
    for (int i = k; i < (*tailleMain) - 1; i++) {
        main[i] = main[i + 1];
    }
    (*tailleMain)--; //Réduit taille main comme carte en moins
}

// Vérifie si une main contient une carte jouable
int a_une_carte_jouable(int main[], int tailleMain, int top) {
    for (int i = 0; i < tailleMain; i++) {
        if (peut_jouer(main[i], top)) return 1;
    }
    return 0;


}

// Affiche la main du joueur
void afficher_main_joueur() {
    printf("\nVotre main :\n");
    for (int i = 0; i < tailleMainJoueur; i++) {
        printf("  %2d : ", i + 1);
        afficher_carte(mainJoueur[i]);
        printf("\n");
    }
}

// +2
void effet_plus_2(int main[], int* tailleMain) {
    printf("+2 : vous devez piocher 2 cartes\n");
    for (int i = 0; i < 2; i++) {
        piocher(main, tailleMain, 1);
    }
}

// +4
void effet_plus_4(int main[], int* tailleMain) {
    printf("+4 : vous devez piocher 4 cartes\n");
    for (int i = 0; i < 4; i++) {
        piocher(main, tailleMain, 1);
    }
}

// Tour du joueur
void tour_joueur() {
    int choix;

    printf("\n----------------------------------\n");
    printf("Votre tour\n");
    printf("Carte au centre : ");
    afficher_carte(topCard);
    printf("\n");

    afficher_main_joueur();

    // Si la carte est +2 ou +4 on applique l'effet et on change la carte du dessus après
    if (valeur(topCard) == 10) { // +2
        effet_plus_2(mainJoueur, &tailleMainJoueur);
        printf("Votre tour est passé à cause du +2\n");
        // Changement de carte du dessus
        topCard = deck[indexPioche];
        indexPioche++;
        return;
    }
    if (valeur(topCard) == 11) { // +4
        effet_plus_4(mainJoueur, &tailleMainJoueur);
        printf("Votre tour est passé à cause du +4\n");
        topCard = deck[indexPioche];
        indexPioche++;
        return;
    }

    // Si pas de carte jouable, piocher
    if (!a_une_carte_jouable(mainJoueur, tailleMainJoueur, topCard)) {
        printf("Aucune carte jouable, vous devez piocher\n");
        piocher(mainJoueur, &tailleMainJoueur, 1);
        return;
    }

    // Demande au joueur de choisir
    while (1) {
        printf("Choisissez une carte a jouer (1..%d) ou 0 pour piocher : ", tailleMainJoueur);
        if (scanf("%d", &choix) != 1) {
            int c;
			while ((c = getchar()) != '\n' && c != EOF) {} // La fonction de stackoverflow utilisée pour vider le buffer
            printf("Entree invalide\n");
            continue;
        }

        if (choix == 0) {
            piocher(mainJoueur, &tailleMainJoueur, 1);
            break;
        }

        if (choix < 1 || choix > tailleMainJoueur) {
            printf("Numero invalide.\n");
            continue;
        }

        int idx = choix - 1;
        int carte = mainJoueur[idx];

        if (!peut_jouer(carte, topCard)) {
            printf("Cette carte ne peut pas etre jouee\n");
            continue;
        }

        printf("Vous jouez : ");
        afficher_carte(carte);
        printf("\n");

        topCard = carte;
        enlever_carte(mainJoueur, &tailleMainJoueur, idx);
        break;
    }
}

// Tour du bot
void tour_bot() {
    printf("\n----------------------------------\n");
    printf("Tour du bot\n");
    printf("Carte au centre : ");
    afficher_carte(topCard);
    printf("\n");

    //  Si la carte est +2 ou +4 on applique l'effet et on change la carte du dessus après
    if (valeur(topCard) == 10) { // +2
        effet_plus_2(mainBot, &tailleMainBot);
        printf("Le bot passe son tour à cause de l'effet +2\n");
        topCard = deck[indexPioche];
        indexPioche++;
        return;
    }
    if (valeur(topCard) == 11) { // +4
        effet_plus_4(mainBot, &tailleMainBot);
        printf("Le bot passe son tour à cause de l'effet +4\n");
        topCard = deck[indexPioche];
        indexPioche++;
        return;
    }

    // Cherche une carte jouable
    for (int i = 0; i < tailleMainBot; i++) {
        if (peut_jouer(mainBot[i], topCard)) {
            printf("Bot joue : ");
            afficher_carte(mainBot[i]);
            printf("\n");
            topCard = mainBot[i];
            enlever_carte(mainBot, &tailleMainBot, i);
            printf("Le bot a maintenant %d cartes\n", tailleMainBot);
            return;
        }
    }

    // Sinon pioche
    printf("Le bot pioche.\n");
    piocher(mainBot, &tailleMainBot, 0);
    printf("Le bot a maintenant %d cartes\n", tailleMainBot);
}

int main() {
    srand((unsigned)time(NULL)); // Initialisation du générateur de nombres aléatoires

    initialiser_deck();
    melanger_deck();

    // Distribuer les cartes aux joueurs
    for (int i = 0; i < CARTES_PAR_JOUEUR; i++) { 
        piocher(mainJoueur, &tailleMainJoueur, 0);
        piocher(mainBot, &tailleMainBot, 0);
    }

    // Première carte
    topCard = deck[indexPioche];
    indexPioche++;

    int joueurCourant = 0; // 0=humain, 1=bot
    int vainqueur = -1;

    while (1) {
        if (joueurCourant == 0) {
            tour_joueur();
            if (tailleMainJoueur == 0) {
                vainqueur = 0;
                break;
            }
        }
        else {
            tour_bot();
            if (tailleMainBot == 0) {
                vainqueur = 1;
                break;
            }
        }
        joueurCourant = 1 - joueurCourant;
    }

    printf("\n----------------------------------\n");
    if (vainqueur == 0) {
        printf("Vous avez gagne\n");
    }
    else {
        printf("Le bot a gagne\n");
    }
    return 0;
}
