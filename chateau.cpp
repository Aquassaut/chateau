/**
 * Projet d'algorithmique et programmation numero 1
 * "Jeu des châteaux"
 * 
 * Jérémy Autran - S1A1
 * repo git privé : https://github.com/Aquassaut/chateau.git
 */

//Partie include
#include <math.h> //Pour la modélisation d'éléments physiques
#include <string> 
#include <iostream>
#include <DrawingWindow.h> //Bibliothèque graphique
#include <cstdlib> //Pour générer un nombre pseudo-aléatoire
#include <time.h> //Pour initialiser le randomizer


//Namespace
using namespace std;

//Prototypes des fonctions
void setUpVent();
void setUpColine();
void setUpVariables();
void exemple1(DrawingWindow &w);

//Implémentation des fonctions
int main() {

}

void setUpVariables() {
    srand(time(NULL)); //Initialise le random
    setUpVent();
    setUpColine();
}

void setUpVent() {
    int ventMax = 50; //CONFIG

    //On cherche à obtenir une valeur entre -ventMax et ventMax, du coup
    //on prend une valeur entre 0 et ventMax avec le modulo, et on lui affecte un signe
    //en multipliant cette valeur par -1 exposant rand(). Si rand est pair, on aura -1*total;
    //Si rand est impair, on aura 1*total
    
    int vent = (rand() % ventMax) * (pow(-1, rand()));
    cout << "le vent vaut : " << vent << endl; // DEBUG
}

void setUpColine() {
    int hauteurMin = 20, hauteurMax = 80; //CONFIG
    int largeurMin = 10, largeurMax = 40; //CONFIG

    //On cherche à obtenir deux valeurs, entre min et max, du coup, on prend une valeur
    //entre 0 et la range disponible (max - min) avec le modulo, et on lui ajoute min
    //de façon à avoir une valeur aléatoire entre min et max

    int hauteur = hauteurMin + (rand() % (hauteurMax - hauteurMin));
    int largeur = largeurMin + (rand() % (largeurMax - largeurMin));
    cout << "la hauteur vaut : " << hauteur << endl; //DEBUG
    cout << "la largeur vaut : " << largeur << endl; //DEBUG
}
// On a deux chateaux, un en (-p, 0), un en (p, 0)
// On a une colline au milieu, sa courbe vaut : y = randHauteur*(1-On a du vent pow((2x/randLargeur, 2)))
// on a un vent de valeur random entre -ventMax et ventMax
// Ca devrait être suivante pour savoir quelles variables locales on va devoir créer :
// On aura donc une fonction qui va garder les variables locales, après création par des fonction spécifiques
//
//rand() donne un gros nombre, genre 9 chiffres


