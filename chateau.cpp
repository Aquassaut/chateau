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

//constantes
#define GRAV 9.81
#define KFROT 0.001
#define REFRESH 0.05
#define VENT_MAX 10

//Prototypes des fonctions
int setUpVent(DrawingWindow&);
void colline(DrawingWindow&, float[]);
void collineRand(float&, float&);
void staticEnv(DrawingWindow&, float[]);
void fenetreDeJeu(DrawingWindow&);
void chatBowser(DrawingWindow&);
void chatMario(DrawingWindow&);
int playerMove(int, float[], DrawingWindow&);
void prompt(int&, int&); 
int ventRand();
void barreBas(DrawingWindow&);
void nPosition(float[], float[], int, int);
int checkCollision(float[], float[]);


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
int main(int argc, char *argv[]) {
    QApplication application(argc, argv);
    DrawingWindow window1(fenetreDeJeu, 640, 480);
    window1.setWindowTitle("Detruisez le chateau de Bowser");   
    window1.show();
    application.exec();
    return 0;
}

void fenetreDeJeu(DrawingWindow &w) {
    //la largeur vaut 640
    //la hauteur vaut 480   
    int compteur = 1; // Son mod 2 donnera le numéro du joueur
    float ventColHColL[3];
    int collision = 0; //pourra faire sortir de la boucle en cas de victoire
    staticEnv(w, ventColHColL); //Dessine l'environnement statique (bckg, colline, chateaux)
    while(collision != 1 && collision != 2) {
        compteur +=1;
        cout << "A votre tour de joueur, Player " << compteur%2+1 << endl;
        collision = playerMove(compteur%2+1, ventColHColL, w);
    }
    if(collision == 1)
        cout << "Player 1 a gagné !" << endl;
    else if(collision == 2)
        cout << "Player 2 a gagné !" << endl;
}
   /**
    * A partir de l'angle et de la force, il faut :
    *  1 choisir une position de départ avec les coordonnées du chateau du player courant
    *  2 pour chaque interval de temps, calculer la nouvelle position
    *  3 pour chaque nouvelle position, vérifier si il y a eu une collision
    *  4 en cas de collision, arreter la boucle
    *  5 en cas de collision, déclarer une victoire
    */

int playerMove(int player, float ventColHColL[], DrawingWindow &w) {
    int vent = ventColHColL[0];
    int angle, force, collision = 0;
    float coord [2];
    float vitesse [2];
    prompt(angle, force);
    vitesse[0] = 1.5*force*cos(M_PI*angle/180);
    vitesse[1] = 1.5*force*sin(M_PI*angle/180);
    cout << "Les vitesses initiales sont " << vitesse[0] << " et " << vitesse[1] << endl;
//    coord[1] = w.height-71; //ordonnée initialle : juste au dessus du drapeau
    coord[1] = 71;
    if (player == 1) {
        coord[0] = -w.width/2+38; //38; //juste à droite de l'abs du drapeau de Bowser

    }
    else {
        coord[0] = w.width/2-55; //juste à gauche de l'abs du drapeau de Mario
    }
    w.setColor("black");
    while(collision == 0) {
        //dessin de la position
        w.drawCircle((int)(w.width/2+coord[0]), (int)(w.height-coord[1]), 3);
        //check de collision
        collision = checkCollision(coord, ventColHColL);
        //calcul de la nouvelle position
        nPosition(coord, vitesse, vent, player);
        //attendre avant le prochain refresh
        w.msleep(20);
        cout << "collision vaut : " << collision << endl;
    } 
    return collision;
}

   /**
    *   Code collision :
    *   Pas de collision : 0
    *   collision chateau p2 : 1 
    *   collision chateau p1 : 2
    *   collision sol : 3
    *   collision colline : 4
    */

int checkCollision(float p[], float vCC[]) { 
//TODO : Faire des trucs pour les chateaux
    bool col = 0;
    if (p[1] <= 30)
        col = 3;
    else if ((vCC[1]*(1-(2*p[0]/vCC[2])*(2*p[0]/vCC[2]))) > p[1]-31)
        col = 4;
    else if (p[0] >= -300 && p[0] <= -260 && p[1] <= 70) //X entre 20 et 60 et y sous 70
        col = 2;
    else if (p[0] >= 260 && p[0] <= 300 && p[1] <= 70)
        col = 1;
    return col;
}


void nPosition(float p[], float v[], int vVent, int player) {
    float vr = sqrt(pow(v[0]-vVent, 2) + pow(v[1], 2));
    //Update des positions
    
    p[1] += v[1] * REFRESH;
    if (player == 1) {
        p[0] += v[0] * REFRESH;
    }
    else {
        p[0] -= v[0] * REFRESH;
        vVent = -vVent;
    }
    //update des vitesses prises en compte à la prochaine itération
    v[0] += (REFRESH * (-KFROT * (vr) * (v[0] - vVent)));
    v[1] += (REFRESH * (-KFROT * (vr) * v[1] - GRAV));
    //DEBUG
    cout << "VR vaut : " << vr << endl;
    cout << "Vitesse : " << v[0] << "," << v[1] << endl;
    cout << "Pos : " << p[0] << "," << p[1] << endl;
}


void staticEnv(DrawingWindow &w, float ventColHColL[]) {
    w.setBgColor("lightcyan");
    w.clearGraph();
    srand(time(NULL)); //Initialise le random
    barreBas(w);
    colline(w, ventColHColL); //dessine la colline
    chatBowser(w);
    chatMario(w);
    ventColHColL[0] = setUpVent(w);
}

void colline(DrawingWindow &w, float ventColHColL[]) {
    //float larg, haut;
    int offHaut = w.height-31;
    collineRand(ventColHColL[1], ventColHColL[2]);
    w.setColor("sienna");
    for (int x = -w.width/2; x <= w.width/2; x+=1) { //x
        for (int y = w.height; y > 0; y-=1) { //y
            if ((ventColHColL[1]*(1-(2*x/ventColHColL[2])*(2*x/ventColHColL[2]))) > y) { 
                w.drawLine(x+w.width/2, offHaut-y, x+w.width/2, offHaut); 
                //w.drawLine(x+w.width/2, w.height-y, x+w.width/2, w.height); 
                //cout << "J'ai dessiné une ligne sous " << (x+(w.width/2)) << "," << (w.height-y) << endl;
                y = 0; //next x
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

int setUpVent(DrawingWindow &w) {
    int vent = ventRand();
    cout << "le vent vaut : " << vent << endl; // DEBUG
    w.setColor("black");
    //Partie horizontale de la flèche sur 30 pixels
    w.drawLine(w.width/2-15, w.height-60, w.width/2+15, w.height-60);
    //Vent vers la gauche
    if (vent < 0) {
        w.drawLine(w.width/2-15, w.height-60, w.width/2-10, w.height-65);
        w.drawLine(w.width/2-15, w.height-60, w.width/2-10, w.height-55);
    }
    else {
        w.drawLine(w.width/2+15, w.height-60, w.width/2+10, w.height-65);
        w.drawLine(w.width/2+15, w.height-60, w.width/2+10, w.height-55);
    }
    return vent;
}

int ventRand() {
    //On cherche à obtenir une valeur entre -ventMax et ventMax, du coup
    //on prend une valeur entre 0 et ventMax avec le modulo, et on lui affecte un signe
    //en multipliant cette valeur par -1 exposant rand(). Si rand est pair, on aura -1*total;
    //Si rand est impair, on aura 1*total
    int vent = (rand() % VENT_MAX) * (pow(-1, rand()));
    return vent;
}


void barreBas(DrawingWindow& w) {
    w.setColor("darkslategrey");
    w.fillRect(0, w.height, w.width, w.height-29);
}


void chatBowser(DrawingWindow &w) {
    int offHaut = w.height-70; //L'ordonnée la plus en haut pour dessiner les chateaux
    int offDroite = 20; //L'abscisse des points les plus à droite du chateau
    //poutre
    w.setColor("darkgrey");
    w.fillRect(offDroite+16, offHaut+00, offDroite+17, offHaut+14);
    //drapeau
    w.setColor("darkred");
    w.fillTriangle(offDroite+15, offHaut+1,
                   offDroite+11, offHaut+1,
                   offDroite+15, offHaut+5);
    //partie haute
    w.setColor("dimgray");
    w.fillRect(offDroite+7, offHaut+12, offDroite+9, offHaut+27);
    w.fillRect(offDroite+31, offHaut+12, offDroite+33, offHaut+27);
    w.fillRect(offDroite+13, offHaut+12, offDroite+16, offHaut+14);
    w.fillRect(offDroite+24, offHaut+12, offDroite+27, offHaut+14);
    w.fillRect(offDroite+10, offHaut+15, offDroite+30, offHaut+37);
    //fenetres
    w.setColor("black");
    w.drawPoint(offDroite+10, offHaut+16);
    w.drawPoint(offDroite+30, offHaut+16);
    w.drawPoint(offDroite+27, offHaut+18);
    w.drawPoint(offDroite+13, offHaut+18);
    w.drawLine(offDroite+11, offHaut+17, offDroite+12, offHaut+17);
    w.drawLine(offDroite+28, offHaut+17, offDroite+29, offHaut+17);
    w.drawLine(offDroite+12, offHaut+19, offDroite+13, offHaut+19);
    w.drawLine(offDroite+27, offHaut+19, offDroite+28, offHaut+19);
    w.drawLine(offDroite+11, offHaut+20, offDroite+14, offHaut+20);
    w.drawLine(offDroite+26, offHaut+20, offDroite+29, offHaut+20);
    w.fillRect(offDroite+10, offHaut+21, offDroite+15, offHaut+27);
    w.fillRect(offDroite+25, offHaut+21, offDroite+30, offHaut+27);
    //partie basse
    w.setColor("darkgrey");
    w.fillRect(offDroite+0, offHaut+25, offDroite+1, offHaut+27);
    w.fillRect(offDroite+5, offHaut+25, offDroite+7, offHaut+27);
    w.fillRect(offDroite+12, offHaut+25, offDroite+14, offHaut+27);
    w.fillRect(offDroite+19, offHaut+24, offDroite+21, offHaut+27);
    w.fillRect(offDroite+26, offHaut+25, offDroite+28, offHaut+27);
    w.fillRect(offDroite+33, offHaut+25, offDroite+35, offHaut+27);
    w.fillRect(offDroite+39, offHaut+25, offDroite+40, offHaut+27);
    w.fillRect(offDroite+0, offHaut+28, offDroite+40, offHaut+39);
    //porte
    w.setColor("black");
    w.drawPoint(offDroite+17, offHaut+33);
    w.drawPoint(offDroite+20, offHaut+33);
    w.drawPoint(offDroite+23, offHaut+33);
    w.drawLine(offDroite+17, offHaut+34, offDroite+23, offHaut+34);
    w.fillRect(offDroite+16, offHaut+35, offDroite+24, offHaut+39); 
}


void chatMario(DrawingWindow &w) {
    int offHaut = w.height-70; //L'ordonnée la plus en haut pour dessiner les chateaux
    int offDroite = w.width-60; //L'abscisse des points les plus à droite du chateau
    //poteau
    w.setColor("darkgrey");
    w.fillRect(offDroite+5, offHaut+0, offDroite+6, offHaut+35);
    w.fillRect(offDroite+4, offHaut+36, offDroite+7, offHaut+39);
    //drapeau
    w.setColor("blue");
    w.fillTriangle(offDroite+0, offHaut+1,
                   offDroite+4, offHaut+1,
                   offDroite+4, offHaut+5);
    //petits rectangles
    w.setColor("darkgrey");
    w.fillRect(offDroite+10, offHaut+24, offDroite+40, offHaut+39);
    w.fillRect(offDroite+16, offHaut+14, offDroite+34, offHaut+23);
    w.fillRect(offDroite+10, offHaut+22, offDroite+13, offHaut+23);
    w.fillRect(offDroite+37, offHaut+22, offDroite+40, offHaut+23);
    w.fillRect(offDroite+16, offHaut+11, offDroite+20, offHaut+13);
    w.fillRect(offDroite+23, offHaut+11, offDroite+27, offHaut+13);
    w.fillRect(offDroite+30, offHaut+11, offDroite+34, offHaut+13);
    //fenetres
    w.setColor("black");
    w.fillRect(offDroite+19, offHaut+18, offDroite+21, offHaut+24);
    w.fillRect(offDroite+29, offHaut+18, offDroite+31, offHaut+24);
    //porte
    w.fillRect(offDroite+22, offHaut+31, offDroite+28, offHaut+39);
}

void prompt(int& angle, int& force) {
    bool erreur = false;
    cout << "Entrez un angle" << endl;
    do {
        if (erreur)
            cout << "Cet angle n'est pas valide, recommencez" << endl;
        cin >> angle;
        erreur = true;
    }
    while(angle > 90 || angle < 0);
    erreur = false;
    cout << "Entrez une force en pourcent" << endl;
    do {
        if (erreur)
            cout << "Cette force n'est pas valide, recommencez" << endl;
        cin >> force;
        erreur = true;
    }
    while(force > 100 || force < 0);
}
