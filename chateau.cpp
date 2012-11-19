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


//Namespace
using namespace std;

//Prototypes des fonctions


int main() {
    for (int i = 0; i < 1000; i += 1) {
        cout << rand() << ", ";
        if (i%10 == 0)
            cout << endl;
    }
}

// On a deux chateaux, un en (-p, 0), un en (p, 0)
// On a une colline au milieu, sa courbe vaut : y = randHauteur*(1-On a du vent pow((2x/randLargeur, 2)))
// on a un vent de valeur random entre -ventMax et ventMax
// Ca devrait être suivante pour savoir quelles variables locales on va devoir créer :
// On aura donc une fonction qui va garder les variables locales, après création par des fonction spécifiques
//


