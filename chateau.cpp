/**
 * Projet d'algorithmique et programmation numero 1
 * "Jeu des châteaux"
 * 
 * Jérémy Autran - S1A1
 * repo git privé : https://github.com/Aquassaut/chateau.git
 */

//Partie include
#include <QApplication>
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
void colline(DrawingWindow&);
void collineRand(float&, float&);
void staticEnv(DrawingWindow&);
void fenetreDeJeu(DrawingWindow&);
void chatBowser(DrawingWindow&);
void chatMario(DrawingWindow&);

/*BLOC NOTE
* On a deux chateaux, un en (-p, 0), un en (p, 0)
* On a une colline au milieu, sa courbe vaut : y = randHauteur*(1-On a du vent pow((2x/randLargeur, 2)))
* on a un vent de valeur random entre -ventMax et ventMax
* Ca devrait être suivante pour savoir quelles variables locales on va devoir créer :
* On aura donc une fonction qui va garder les variables locales, après création par des fonction spécifiques
*
* rand() donne un gros nombre, genre 9 chiffres
*
*
*/

//Implémentation des fonctions
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    DrawingWindow window1(fenetreDeJeu, 640, 480);
    window1.setWindowTitle("Detruisez le chateau de Bowser");   
    window1.show();
    application.exec();
    return 0;
}

void fenetreDeJeu(DrawingWindow &w) {
    staticEnv(w); //Dessine l'environnement statique (bckg, colline, chateaux)
    //la largeur vaut 640
    //la hauteur vaut 480   
}

void staticEnv(DrawingWindow &w) {
    w.setBgColor("powderblue");
    w.clearGraph();
    srand(time(NULL)); //Initialise le random
    colline(w); //dessine la colline
    chatBowser(w);
    chatMario(w);


    setUpVent();
}

void colline(DrawingWindow &w) {
    float larg, haut;
    collineRand(larg, haut);
    w.setColor("sienna");
    for (int x = -w.width/2; x <= w.width/2; x+=1) { //x
        for (int y = w.height; y > 0; y-=1) { //y
            if ((haut*(1-(2*x/larg)*(2*x/larg))) > y) { 
                w.drawLine((x+(w.width/2)), (w.height-y),(x+(w.width/2)), w.height); 
                //cout << "J'ai dessiné une ligne sous " << (x+(w.width/2)) << "," << (w.height-y) << endl;
                y = 0;
            }
        }
    }
}

void collineRand(float& largeur, float& hauteur) {
    int hauteurMin = 80, hauteurMax = 300; //CONFIG
    int largeurMin = 50, largeurMax = 200; //CONFIG
    //On cherche à obtenir deux valeurs, entre min et max, du coup, on prend une valeur
    //entre 0 et la range disponible (max - min) avec le modulo, et on lui ajoute min
    //de façon à avoir une valeur aléatoire entre min et max
    hauteur = hauteurMin + (rand() % (hauteurMax - hauteurMin));
    largeur = largeurMin + (rand() % (largeurMax - largeurMin));
    cout << "la hauteur vaut : " << hauteur << endl; //DEBUG
    cout << "la largeur vaut : " << largeur << endl; //DEBUG
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


void chatBowser(DrawingWindow &w) {
    //poutre
    w.setColor("silver");
    w.fillRect(20+16, w.height-40+00, 20+17, w.height-40+14);
    //drapeau
    w.setColor("darkred");
    w.fillTriangle(20+15, w.height-40+01, 20+11, w.height-40+01, 20+15, w.height-40+05);
    //partie haute
    w.setColor("dimgray");
    w.fillRect(20+7, w.height-40+12, 20+9, w.height-40+27);
    w.fillRect(20+31, w.height-40+12, 20+33, w.height-40+27);
    w.fillRect(20+13, w.height-40+12, 20+16, w.height-40+14);
    w.fillRect(20+24, w.height-40+12, 20+27, w.height-40+14);
    w.fillRect(20+10, w.height-40+15, 20+30, w.height-40+37);
    //fenetres
    w.setColor("black");
    w.drawPoint(20+10, w.height-40+16);
    w.drawPoint(20+30, w.height-40+16);
    w.drawPoint(20+27, w.height-40+18);
    w.drawPoint(20+13, w.height-40+18);
    w.drawLine(20+11, w.height-40+17, 20+12, w.height-40+17);
    w.drawLine(20+28, w.height-40+17, 20+29, w.height-40+17);
    w.drawLine(20+12, w.height-40+19, 20+13, w.height-40+19);
    w.drawLine(20+27, w.height-40+19, 20+28, w.height-40+19);
    w.drawLine(20+11, w.height-40+20, 20+14, w.height-40+20);
    w.drawLine(20+26, w.height-40+20, 20+29, w.height-40+20);
    w.fillRect(20+10, w.height-40+21, 20+15, w.height-40+27);
    w.fillRect(20+25, w.height-40+21, 20+30, w.height-40+27);
    //partie basse
    w.setColor("silver");
    w.fillRect(20+0, w.height-40+25, 20+1, w.height-40+27);
    w.fillRect(20+5, w.height-40+25, 20+7, w.height-40+27);
    w.fillRect(20+12, w.height-40+25, 20+14, w.height-40+27);
    w.fillRect(20+19, w.height-40+24, 20+21, w.height-40+27);
    w.fillRect(20+26, w.height-40+25, 20+28, w.height-40+27);
    w.fillRect(20+33, w.height-40+25, 20+35, w.height-40+27);
    w.fillRect(20+39, w.height-40+25, 20+40, w.height-40+27);
    w.fillRect(20+0, w.height-40+28, 20+40, w.height-40+39);
    //porte
    w.setColor("black");
    w.drawPoint(20+17, w.height-40+33);
    w.drawPoint(20+20, w.height-40+33);
    w.drawPoint(20+23, w.height-40+33);
    w.drawLine(20+17, w.height-40+34, 20+23, w.height-40+34);
    w.fillRect(20+16, w.height-40+35, 20+24, w.height-40+39); 
}

void chatMario(DrawingWindow &w) {
    //poteau
    w.setColor("silver");
    w.fillRect( w.width-60+5, w.height-40+6, w.width-60+6, w.height-40+35);
    w.fillRect( w.width-60+4, w.height-40+36, w.width-60+7, w.height-40+39);
    //drapeau
    w.setColor("blue");
    w.fillTriangle( w.width-60+0, w.height-40+7, w.width-60+4, w.height-40+7, w.width-60+4, w.height-40+11);
    //petits rectangles
    w.setColor("silver");
    w.fillRect( w.width-60+10, w.height-40+24, w.width-60+40, w.height-40+39);
    w.fillRect( w.width-60+16, w.height-40+14, w.width-60+34, w.height-40+23);
    w.fillRect( w.width-60+10, w.height-40+22, w.width-60+13, w.height-40+23);
    w.fillRect( w.width-60+37, w.height-40+22, w.width-60+40, w.height-40+23);
    w.fillRect( w.width-60+16, w.height-40+11, w.width-60+20, w.height-40+13);
    w.fillRect( w.width-60+23, w.height-40+11, w.width-60+27, w.height-40+13);
    w.fillRect( w.width-60+30, w.height-40+11, w.width-60+34, w.height-40+13);
    //fenetres
    w.setColor("black");
    w.fillRect( w.width-60+19, w.height-40+18, w.width-60+21, w.height-40+24);
    w.fillRect( w.width-60+29, w.height-40+18, w.width-60+31, w.height-40+24);
    //porte
    w.fillRect( w.width-60+22, w.height-40+31, w.width-60+28, w.height-40+39);
}
