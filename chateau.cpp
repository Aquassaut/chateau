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


// SALUT FX

//Namespace
using namespace std;

//constantes
#define GRAV 9.81
#define KFROT 0.001
#define REFRESH 0.05
#define VENT_MAX 10
#define F_HAUT 480
#define F_LARG 640


#define HCOL_MIN 80
#define HCOL_MAX 300
#define LCOL_MIN 50
#define LCOL_MAX 200

//Prototypes des fonctions
float setUpVent(DrawingWindow&);
void colline(DrawingWindow&, float[]);
void collineRand(float&, float&);
void staticEnv(DrawingWindow&, float[]);
void fenetreDeJeu(DrawingWindow&);
void chatBowser(DrawingWindow&);
void chatMario(DrawingWindow&);
int playerMove(int, float[], DrawingWindow&);
void prompt(int&, int&); 
float ventRand();
void barreBas(DrawingWindow&);
void nPosition(float[], float[], int, int);
int checkCollision(float[], float[]);
int convAbs(float);
int convOrd(float);
void tortue(DrawingWindow &w, float x, float y);

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
    DrawingWindow window1(fenetreDeJeu, F_LARG, F_HAUT);
    window1.setWindowTitle("Detruisez le chateau de Bowser");   
    window1.show();
    application.exec();
    return 0;
}

void fenetreDeJeu(DrawingWindow &w) {
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
    coord[1] = 41;
    if (player == 1) {
        coord[0] = -w.width/2+38; //38; //juste à droite de l'abs du drapeau de Bowser

    }
    else {
        coord[0] = w.width/2-55; //juste à gauche de l'abs du drapeau de Mario
    }
    int preCoul[16][12]; //Les couleurs précédentes 
    float coordTortue[2];
    while(collision <= 0) {
        collision = checkCollision(coord, ventColHColL);
        //dessin de la position
        for (int x = 0; x <= 16; x += 1) { //Pos min et max de x de la tortue
            for(int y = 12; y >= 0; y -= 1) {
                coordTortue[0] = coord[0] + x - 7;
                coordTortue[1] = coord[1] + y + 1;
                int collValide = checkCollision(coordTortue, ventColHColL);
                if(collValide != -1)
                    preCoul[x][y] = w.getPointColor(convAbs(coordTortue[0]), convOrd(coordTortue[1]));
            }
        }
        //
        tortue(w, coord[0], coord[1]);
        //check de collision
        w.msleep(60);
        for (int x = 0; x <= 16; x += 1) { //Pos min et max de x de la tortue
            for(int y = 0; x <= 12; x += 1) {
                coordTortue[0] = coord[0] + x - 7;
                coordTortue[1] = coord[1] + y + 1;
                w.setColor(preCoul[x][y]);
                w.drawPoint(convAbs(coordTortue[0]), convOrd(coordTortue[1]));
                //w.drawCircle(convAbs(coordTortue[0]), convOrd(coordTortue[1]), 3);
            }
        }
        //w.drawPoint(convAbs(coord[0]), convOrd(coord[1]));
        //calcul de la nouvelle position
        nPosition(coord, vitesse, vent, player);
        //attendre avant le prochain refresh
        //cout << "collision vaut : " << collision << endl;
    } 
    return collision;
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
    collineRand(ventColHColL[1], ventColHColL[2]);
    w.setColor("sienna");
    for (int x = 0; x <= ventColHColL[2]/2; x+=1) { //x
        for (int y = F_HAUT; y > 0; y-=1) { //y 
            if ((ventColHColL[1]*(1-pow(2*x/ventColHColL[2], 2))) > y) { 
                w.drawLine(convAbs(x), convOrd(y), convAbs(x), convOrd(0));
                w.drawLine(convAbs(-x), convOrd(y), convAbs(-x), convOrd(0));
                y = 0; //next x
            }
        }
    }
}

void tortue(DrawingWindow &w, float x, float y) {
    //Le milieu de la carapace
    w.setColor("darkgreen");
    w.fillRect(convAbs(x - 3), convOrd(y + 11), convAbs(x + 4), convOrd(y + 9));
    w.fillRect(convAbs(x - 6), convOrd(y + 8), convAbs(x + 7), convOrd(y + 4));
    //Les cotes de la carapace
    w.setColor("green");
    w.fillRect(convAbs(x - 2), convOrd(y + 9), convAbs(x + 3), convOrd(y + 7));
    //le bas de la carapace
    w.setColor("White");
    w.fillRect(convAbs(x - 6), convOrd(y + 4), convAbs(x - 2), convOrd(y + 2));
    w.fillRect(convAbs(x + 3), convOrd(y + 4), convAbs(x + 7), convOrd(y + 2));
    w.fillRect(convAbs(x - 1), convOrd(y + 2), convAbs(x + 2), convOrd(y + 1));
    //les contours
    w.setColor("black");
    w.drawLine(convAbs(x - 1), convOrd(y), convAbs(x + 2), convOrd(y));
    w.drawLine(convAbs(x - 3), convOrd(y + 1), convAbs(x - 2), convOrd(y + 1));
    w.drawLine(convAbs(x + 3), convOrd(y + 1), convAbs(x + 4), convOrd(y + 1));
    w.drawLine(convAbs(x - 6), convOrd(y + 2), convAbs(x - 3), convOrd(y + 2));
    w.drawLine(convAbs(x + 4), convOrd(y + 2), convAbs(x + 7), convOrd(y + 2));
    w.drawPoint(convAbs(x - 6), convOrd(y + 3));
    w.drawPoint(convAbs(x + 7), convOrd(y + 3));
    w.drawLine(convAbs(x - 7), convOrd(y + 7), convAbs(x - 7), convOrd(y + 3));
    w.drawLine(convAbs(x + 8), convOrd(y + 7), convAbs(x + 8), convOrd(y + 3));
    w.drawLine(convAbs(x - 1), convOrd(y + 3), convAbs(x + 2), convOrd(y + 3));
    w.drawLine(convAbs(x - 4), convOrd(y + 4), convAbs(x - 2), convOrd(y + 4));
    w.drawLine(convAbs(x + 3), convOrd(y + 4), convAbs(x + 5), convOrd(y + 4));
    w.drawPoint(convAbs(x - 5), convOrd(y + 5));
    w.drawPoint(convAbs(x + 6), convOrd(y + 5));
    w.drawPoint(convAbs(x - 2), convOrd(y + 5));
    w.drawPoint(convAbs(x + 3), convOrd(y + 5));
    w.drawLine(convAbs(x - 6), convOrd(y + 9), convAbs(x - 6), convOrd(y + 7));
    w.drawLine(convAbs(x + 7), convOrd(y + 9), convAbs(x + 7), convOrd(y + 7));
    w.drawRect(convAbs(x - 5), convOrd(y + 10), convAbs(x - 4), convOrd(y + 9));
    w.drawRect(convAbs(x + 5), convOrd(y + 10), convAbs(x + 6), convOrd(y + 9));
    w.drawLine(convAbs(x - 2), convOrd(y + 12), convAbs(x + 3), convOrd(y + 12));
    w.drawLine(convAbs(x - 3), convOrd(y + 11), convAbs(x - 2), convOrd(y + 11));
    w.drawLine(convAbs(x + 3), convOrd(y + 11), convAbs(x + 4), convOrd(y + 11));
    w.drawLine(convAbs(x - 6), convOrd(y + 5), convAbs(x - 1), convOrd(y + 10));
    w.drawLine(convAbs(x + 2), convOrd(y + 10), convAbs(x + 7), convOrd(y + 5));
    w.drawLine(convAbs(x - 2), convOrd(y + 7), convAbs(x - 1), convOrd(y + 6));
    w.drawLine(convAbs(x + 3), convOrd(y + 7), convAbs(x + 2), convOrd(y + 6));
    w.drawLine(convAbs(x), convOrd(y + 6), convAbs(x + 1), convOrd(y + 6));
    w.drawLine(convAbs(x), convOrd(y + 10), convAbs(x + 1), convOrd(y + 10));
}

void chatBowser(DrawingWindow &w) {
    //poutre
    w.setColor("darkgrey");
    w.fillRect(convAbs(-284), convOrd(40), convAbs(-283), convOrd(26));
    //drapeau
    w.setColor("darkred");
    w.fillTriangle(convAbs(-285), convOrd(39),
                    convAbs(-289), convOrd(39),
                    convAbs(-285), convOrd(35));
    //partie haute
    w.setColor("dimgray");
    w.fillRect(convAbs(-293), convOrd(28), convAbs(-291), convOrd(13));
    w.fillRect(convAbs(-269), convOrd(28), convAbs(-267), convOrd(13));
    w.fillRect(convAbs(-287), convOrd(28), convAbs(-284), convOrd(26));
    w.fillRect(convAbs(-276), convOrd(28), convAbs(-273), convOrd(26));
    w.fillRect(convAbs(-290), convOrd(25), convAbs(-270), convOrd(3));
    //fenetres
    w.setColor("black");
    w.drawPoint(convAbs(-290), convOrd(24));
    w.drawPoint(convAbs(-270), convOrd(24));
    w.drawPoint(convAbs(-273), convOrd(22));
    w.drawPoint(convAbs(-287), convOrd(22));
    w.drawLine(convAbs(-289), convOrd(23), convAbs(-288), convOrd(23));
    w.drawLine(convAbs(-272), convOrd(23), convAbs(-271), convOrd(23));
    w.drawLine(convAbs(-288), convOrd(21), convAbs(-287), convOrd(21));
    w.drawLine(convAbs(-273), convOrd(21), convAbs(-272), convOrd(21));
    w.drawLine(convAbs(-289), convOrd(20), convAbs(-286), convOrd(20));
    w.drawLine(convAbs(-274), convOrd(20), convAbs(-271), convOrd(20));
    w.fillRect(convAbs(-290), convOrd(19), convAbs(-285), convOrd(13));
    w.fillRect(convAbs(-275), convOrd(19), convAbs(-270), convOrd(13));
    //partie basse
    w.setColor("darkgrey");
    w.fillRect(convAbs(-300), convOrd(15), convAbs(-299), convOrd(13));
    w.fillRect(convAbs(-295), convOrd(15), convAbs(-293), convOrd(13));
    w.fillRect(convAbs(-288), convOrd(15), convAbs(-286), convOrd(13));
    w.fillRect(convAbs(-281), convOrd(16), convAbs(-279), convOrd(13));
    w.fillRect(convAbs(-274), convOrd(15), convAbs(-272), convOrd(13));
    w.fillRect(convAbs(-267), convOrd(15), convAbs(-265), convOrd(13));
    w.fillRect(convAbs(-261), convOrd(15), convAbs(-260), convOrd(13));
    w.fillRect(convAbs(-300), convOrd(12), convAbs(-260), convOrd(1));
    //porte
    w.setColor("black");
    w.drawPoint(convAbs(-283), convOrd(7));
    w.drawPoint(convAbs(-280), convOrd(7));
    w.drawPoint(convAbs(-277), convOrd(7));
    w.drawLine(convAbs(-283), convOrd(6), convAbs(-277), convOrd(6));
    w.fillRect(convAbs(-284), convOrd(5), convAbs(-276), convOrd(1));
}

void chatMario(DrawingWindow &w) {
    w.setColor("darkgrey");
    w.fillRect(convAbs(265), convOrd(40), convAbs(266), convOrd(5));
    w.fillRect(convAbs(264), convOrd(4), convAbs(267), convOrd(1));
    //drapeau
    w.setColor("blue");
    w.fillTriangle(convAbs(260), convOrd(39), 
                    convAbs(264), convOrd(39),
                    convAbs(264), convOrd(35));
    //petits rectangles
    w.setColor("darkgrey");
    w.fillRect(convAbs(270), convOrd(16), convAbs(300), convOrd(1));
    w.fillRect(convAbs(276), convOrd(26), convAbs(294), convOrd(17));
    w.fillRect(convAbs(270), convOrd(18), convAbs(273), convOrd(17));
    w.fillRect(convAbs(297), convOrd(18), convAbs(300), convOrd(17));
    w.fillRect(convAbs(276), convOrd(29), convAbs(280), convOrd(27));
    w.fillRect(convAbs(283), convOrd(29), convAbs(287), convOrd(27));
    w.fillRect(convAbs(290), convOrd(29), convAbs(294), convOrd(27));
    //fenetres
    w.setColor("black");
    w.fillRect(convAbs(279), convOrd(22), convAbs(281), convOrd(16));
    w.fillRect(convAbs(289), convOrd(22), convAbs(291), convOrd(16));
    //porte
    w.fillRect(convAbs(282), convOrd(9), convAbs(288), convOrd(1));
}



    /**
     *  Update des positions du projecile. La première étape consiste
     *  à calculer Vr, qui necessite la connaissance de Vx et Vy, puis
     *  mise à jour tour à tour des positions puis des vitesses pour la
     *  prochaine itération. Le calcul de l'acceleration est calculée
     *  directement dans la mise à jour des vitesse pour économiser une
     *  variable.
     *  La position en x et la prise en compte du vent dépendent du joueur
     *  projettant le projectile.
     */

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
    v[0] += (REFRESH * (-KFROT * (vr) * (v[0] - vVent)));
    v[1] += (REFRESH * (-KFROT * (vr) * v[1] - GRAV));
    //cout << "VR vaut : " << vr << endl; //DEBUG
    //cout << "Vitesse : " << v[0] << "," << v[1] << endl; //DEBUG
    //cout << "Pos : " << p[0] << "," << p[1] << endl; //DEBUG
}


   /**
    *   renvoie un code collision selon le type de collision rencontré
    *   Code collision :
    *   Pas de collision : 0
    *   collision chateau p2 : 1 
    *   collision chateau p1 : 2
    *   collision sol : 3
    *   collision colline : 4
    *   hors de la fenêtre : -1
    *   
    *   TODO : ajouter de la précision et prise en compte de la modification de
    *   la hitbox après impact.
    */

int checkCollision(float p[], float vCC[]) { 
    int col = 0;
    if (p[1] <= 0)
        col = 3;
    else if ((vCC[1]*(1-(2*p[0]/vCC[2])*(2*p[0]/vCC[2]))) > p[1]) //-31 ?
        col = 4;
    else if (p[0] >= -300 && p[0] <= -260 && p[1] <= 40) //70? //X entre 20 et 60 et y sous 70
        col = 2;
    else if (p[0] >= 260 && p[0] <= 300 && p[1] <= 40) //70?
        col = 1;
    else if (p[0] >= F_LARG/2 || p[0] <= -F_LARG/2 ||
             p[1] <= -30 || p[1] >= F_HAUT-30)
        col = -1;
    return col;
}


    /**
     *  Reçoit la force du vent, la dessine dans la colline et la renvoie
     *  à la fonction mère.
     *
     *  TODO: peut-être faire en sorte que la classe mère appèle la force 
     *  du vent et l'envoit à cette fonction pour la dessiner. C'est une 
     *  fonction de dessin pas la peine de lui faire faire trop de choses.
     *  Ni de copier 30 fois la même variable par valeur.
     */


float setUpVent(DrawingWindow &w) {
    float vent = ventRand();
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


    /**
     *  Demande à l'utilisateur d'entrer les valeurs pour l'angle
     *  et la force du tir et effectue les tests nécessaires à la
     *  stabilité du programme.
     *
     *  TODO : s'assurer que l'input est uniquement un ou plusieurs
     *  chiffre. Pour l'instant, ça accepte les lettres
     */

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


    /**
     *  dessine la barre du bas/sol
     */

void barreBas(DrawingWindow& w) {
    w.setColor("darkslategrey");
    w.fillRect(convAbs(-F_LARG/2), convOrd(0), convAbs(F_LARG/2), convOrd(-30));
}


    /** 
     *  On cherche à obtenir deux valeurs, entre la valeur min et
     *  la valeur max. Pour ce faire on prend la valeur min et on 
     *  lui ajoute une valeur dans la plage de valeurs disponible
     *  (entre 0 et max - min) avec le modulo.
     */

void collineRand(float& largeur, float& hauteur) {
    hauteur = HCOL_MIN + (rand() % (HCOL_MAX - HCOL_MIN));
    largeur = LCOL_MIN + (rand() % (LCOL_MAX - LCOL_MIN));
    cout << "la hauteur de la colline vaut : " << hauteur << endl; //DEBUG
    cout << "la largeur de la colline vaut : " << largeur << endl; //DEBUG
}


    /**
     *  renvoie la force du vent, entre -VENT_MAX et VENT_MAX.
     *  Cette force est calculée en prenant un nombre pseudo-aléatoire,
     *  en calculant son modulo 100*VENT_MAX, en le divisant par 10 pour
     *  avoir un nombre à virgule (afin d'obtenir une plus grande
     *  fourchette de nombres. On le multiplie ensuite par -1 à la
     *  puissance du nombre pseudo aléatoire afin de définit un signe
     *  aléatoire.
     */

float ventRand() {
    float vent = ((rand() % 100*VENT_MAX) * (pow(-1, rand()))/100);
    return vent;
}

    /**
     *  Renvoie une abscisse utilisable par la bibliothèque graphique à partir 
     *  d'une abscisse d'un système dans lequel l'origine serait au milieu de la
     *  colline, au niveau du sol
     */

int convAbs(float abs) {
    return abs + F_LARG/2;
}

    /**
     *  Renvoie une ordonnée utilisable par la bibliothèque graphique à partir 
     *  d'une ordonnée d'un système dans lequel l'origine serait au milieu de la
     *  colline, au niveau du sol
     */

int convOrd(float ord) {
    return F_HAUT - 30 - ord;
}
