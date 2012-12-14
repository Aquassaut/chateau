/**
 * Projet d'algorithmique et programmation numero 1
 * "Jeu des châteaux"
 * 
 * Jérémy Autran - S1A1
 * repo git privé : https://github.com/Aquassaut/chateau.git
 */

//Partie include
#include <QApplication> //Dependence de la bibliothèque graphique
#include <math.h> //Pour la modélisation d'éléments physiques
#include <sstream> //Pour concatener des int avec des chaines de caractères
#include <iostream> //Pour intéragir avec l'utilisateur par terminal
#include <DrawingWindow.h> //Bibliothèque graphique
#include <cstdlib> //Pour générer un nombre pseudo-aléatoire
#include <time.h> //Pour initialiser le randomizer

//Namespace
using namespace std;

//constantes
#define GRAV 9.81 //Constante gravitationelle
#define KFROT 0.001 //Coefficient de frottement
#define REFRESH 0.1 //Temps de calcul entre deux positions du projectiles 
#define VENT_MAX 10 //Valeur max du vent
#define F_HAUT 480  //hauteur de la fenêtre
#define F_LARG 640 //largeur de la fenêtre
#define HCOL_MIN 230 //Hauteur min de la colline
#define HCOL_MAX 350 //Hauteur max de la colline
#define LCOL_MIN 80 //Largeur min de la colline
#define LCOL_MAX 200 //Largeur max de la colline

//Prototypes des fonctions.
void fenetreDeJeu(DrawingWindow&);
void entreParties(DrawingWindow&, int[], int, bool);
void staticEnv(DrawingWindow&, float[]);
void barreBas(DrawingWindow&);
void colline(DrawingWindow&, float[]);
void collineRand(float&, float&);
void chatBowser(DrawingWindow&);
void chatMario(DrawingWindow&);
void questionBox(DrawingWindow&);
void etoile(DrawingWindow&);
void flamme(DrawingWindow&);
float setUpVent(DrawingWindow&);
float ventRand();
int playerMove(int, float[], DrawingWindow&);
void prompt(int&, int&); 
void coordInit(int p, float coord[]);
int checkCollision(float[], float[]);
void bufferCouleurs(int[16][13], float[], float[], DrawingWindow&);
void tortue(DrawingWindow &w, float[]);
void effaceTortue(int[16][13], float[], DrawingWindow&);
void nPosition(float[], float[], int, int);
int convAbs(float);
int convOrd(float);

//Implémentation des fonctions
int main(int argc, char *argv[]) {
    QApplication application(argc, argv);
    DrawingWindow window1(fenetreDeJeu, F_LARG, F_HAUT);
    window1.setWindowTitle("Detruisez le chateau de Bowser");   
    window1.show();
    application.exec();
    return 0;
}

    /**
     *  Met en place les parties
     */

void fenetreDeJeu(DrawingWindow &w) {
    bool encore = true; //Permet de sortir de la boucle de jeux
    int comptePartie = 0, compteJoueur = 1;//Compte de partie, n° du joueur
    int winJ1J2[2] = {0, 0}; //tableau de scores
    while(encore) {
        comptePartie += 1; 
        encore = false; 
        char reponse;
        //collision determinera quel joueur a gagné. Cette variable ne peut
        //pas être booléenne car elle ne prendrait pas en compte les cas
        //de suicide.
        int collision = 0;
        //Variables passés à staticEnv pour initialisation.
        float ventColHColL[3];
        staticEnv(w, ventColHColL);
        cout << "Manche n° " << comptePartie << endl;
        while(collision != 1 && collision != 2) {
            compteJoueur +=1;
            //la variable est traitée comme ayant la valeur 0 pour le joueur 1
            //et 1 pour le joueur 2. Pour passer de 0 à 1 et de 1 à 2 sur la
            //même instruction sans condition, on utilise le modulo +1.
            //En effet, (0%2)+1 = 1 et (1%2)+1 = 2
            cout << "A votre tour de joueur, Joueur "
                 << compteJoueur%2+1 << endl;
            collision = playerMove(compteJoueur%2+1, ventColHColL, w);
            if (collision != 1 && collision != 2)
                cout << "La précision n'était pas au rendez-vous" << endl;
        }
        //pour déterminer si il y a suicide, on évalue l'égalité ou la non
        //égalité du dernier joueur et du vainqueur
        entreParties(w, winJ1J2, collision, (collision != compteJoueur%2+1));
        cout << "Voulez vous rejouer ? (y/n)" << endl;
        cin >> reponse;
        if (reponse != 'n' && reponse != 'N')
            encore = true;
    }
    //fermeture propre de la fenêtre et du programme
    w.closeGraph();
}

    /**
     *  Annonce le vaiqueur, dessine le tableau de scores et
     *  choisi l'image de fin à afficher
     */

void entreParties(DrawingWindow &w, int winJ1J2[],
                  int dernierWinner, bool suicide) {
    if (suicide)
        cout << "Joueur " << (dernierWinner % 2) + 1
                 << " a mis fin à ses jours !" << endl;
    cout << "Joueur " << dernierWinner << " a gagné !" << endl;
    winJ1J2[dernierWinner-1] += 1;
    //Je n'ai trouvé que la méthode du stringstream pour concatenner une
    //chaine avec un entier.
    stringstream score1, score2;
    score1 << "Joueur 1\n" << winJ1J2[0];
    score2 << "Joueur 2\n" << winJ1J2[1];
    w.setColor("black");
    w.drawText(convAbs(-F_LARG/6-F_LARG/4), convOrd(F_HAUT/2),
               score1.str(), Qt::AlignCenter);
    w.drawText(convAbs(F_LARG/6 + F_LARG/4), convOrd(F_HAUT/2),
               score2.str(), Qt::AlignCenter);
    w.drawText(convAbs(0), convOrd(F_HAUT - 50),
               "Gagnant de la partie :", Qt::AlignCenter);
    questionBox(w);
    if (dernierWinner == 1) {
        flamme(w);
        w.drawText(convAbs(0),convOrd(F_HAUT-180),"Joueur 1",Qt::AlignCenter);
    }
    else {
        etoile(w);
        w.drawText(convAbs(0),convOrd(F_HAUT-180),"Joueur 2",Qt::AlignCenter);
    }
}

    /**
     *  Met au point l'environnement statique, soit le fond, la barre du bas,
     *  la colline, les chateaux et le vent.
     */

void staticEnv(DrawingWindow &w, float ventColHColL[]) {
    QFont maFont("trebuchet", 14); //Font QT utilisée
    w.setFont(maFont); //Selection de la font définie
    w.setBgColor(0x5c94fc);
    w.clearGraph(); //Actualise la couleur de fond
    srand(time(NULL)); //Initialise le random
    barreBas(w);
    colline(w, ventColHColL);
    chatBowser(w);
    chatMario(w);
    ventColHColL[0] = setUpVent(w);
}

    /**
     *  dessine la barre du bas/sol et y affiche les positions respectives
     *  des deux joueurs
     */

void barreBas(DrawingWindow& w) {
    w.setColor(0xc84c0c);
    w.fillRect(convAbs(-F_LARG/2), convOrd(0),
               convAbs(F_LARG/2), convOrd(-30));
    w.setColor("black");
    w.drawText(convAbs(-278), convOrd(-15), "Joueur 1", Qt::AlignCenter);
    w.drawText(convAbs(278), convOrd(-15), "Joueur 2", Qt::AlignCenter);
}

    /**
     *  Dessine la colline parabolique au milieu de la fenêtre. Pour celà
     *  on itère les abscisses du milieu, vers de la droite, jusqu'à la
     *  moitié de la largeur totale de la colline. pour chaque abscisse, on
     *  itère les ordonnées à partir de la hauteur maximale de la colline,
     *  et au premier point trouvé correspondant à la parabole, on trace un
     *  trait de couleur entre celui-ci et le point de même abscisse au
     *  niveau du sol.
     */

void colline(DrawingWindow &w, float ventColHColL[]) {
    collineRand(ventColHColL[2], ventColHColL[1]);
    w.setColor(0x00ab00);
    for (int x = 0; x <= ventColHColL[2]/2; x+=1) {
        for (int y = HCOL_MAX; y > 0; y-=1) {
            if ((ventColHColL[1]*(1-pow(2*x/ventColHColL[2], 2))) >= y) { 
                w.drawLine(convAbs(x), convOrd(y), convAbs(x), convOrd(1));
                w.drawLine(convAbs(-x), convOrd(y), convAbs(-x), convOrd(1));
                y = 0; //next x
            }
        }
    }
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
}

    /**
     *  Dessine le chateau de Bowser (utilisé par Joueur 1)
     */

void chatBowser(DrawingWindow &w) {
    //poutre
    w.setColor(0x6b696b);
    w.fillRect(convAbs(-284), convOrd(40), convAbs(-283), convOrd(26));
    //drapeau
    w.setColor("darkred");
    w.fillTriangle(convAbs(-285), convOrd(39),
                    convAbs(-289), convOrd(39),
                    convAbs(-285), convOrd(35));
    //partie haute
    w.setColor(0x474a46);
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
    w.setColor(0x6b696b);
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

    /**
     * Dessine le chateau de mario (utilisé par Joueur 2)
     */

void chatMario(DrawingWindow &w) {
    w.setColor(0x6b696b);
    w.fillRect(convAbs(265), convOrd(40), convAbs(266), convOrd(5));
    w.fillRect(convAbs(264), convOrd(4), convAbs(267), convOrd(1));
    //drapeau
    w.setColor("blue");
    w.fillTriangle(convAbs(260), convOrd(39), 
                    convAbs(264), convOrd(39),
                    convAbs(264), convOrd(35));
    //petits rectangles
    w.setColor(0x6b696b);
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
     *  Dessine la boite à point d'intérrogation qui s'efface pour
     *  laisser place au symbole du joueur gagnant
     */

void questionBox(DrawingWindow &w) {
    //fond orange
    w.setColor("orange");
    w.fillRect(convAbs(-42), convOrd(402), convAbs(42), convOrd(318));
    //Point d'intérrogation
    w.setColor("white");
    w.fillRect(convAbs(-30), convOrd(384), convAbs(24), convOrd(372));
    w.fillRect(convAbs(-18), convOrd(390), convAbs(24), convOrd(384));
    w.fillRect(convAbs(6), convOrd(372), convAbs(24), convOrd(360));
    w.fillRect(convAbs(-12), convOrd(366), convAbs(6), convOrd(354));
    w.fillRect(convAbs(-12), convOrd(348), convAbs(6), convOrd(336));
    //Bordures
    w.setColor("gray");
    w.fillRect(convAbs(-42), convOrd(408), convAbs(42), convOrd(402));
    w.fillRect(convAbs(42), convOrd(402), convAbs(48), convOrd(318));
    w.fillRect(convAbs(-42), convOrd(318), convAbs(42), convOrd(312));
    w.fillRect(convAbs(-48), convOrd(402), convAbs(-42), convOrd(318));
    w.fillRect(convAbs(-42), convOrd(402), convAbs(-36), convOrd(396));
    w.fillRect(convAbs(-36), convOrd(396), convAbs(-30), convOrd(390));
    w.fillRect(convAbs(30), convOrd(330), convAbs(36), convOrd(324));
    w.fillRect(convAbs(36), convOrd(324), convAbs(42), convOrd(318));
    w.fillRect(convAbs(36), convOrd(402), convAbs(42), convOrd(396));
    w.fillRect(convAbs(30), convOrd(396), convAbs(36), convOrd(390));
    w.fillRect(convAbs(-36), convOrd(330), convAbs(-30), convOrd(324));
    w.fillRect(convAbs(-42), convOrd(324), convAbs(-36), convOrd(318));
    //Ombre du point d'interrogation
    w.fillRect(convAbs(-11), convOrd(378), convAbs(5), convOrd(372));
    w.fillRect(convAbs(25), convOrd(378), convAbs(30), convOrd(360));
    w.fillRect(convAbs(-24), convOrd(371), convAbs(-6), convOrd(367));
    w.fillRect(convAbs(19), convOrd(366), convAbs(24), convOrd(354));
    w.fillRect(convAbs(7), convOrd(360), convAbs(18), convOrd(354));
    w.fillRect(convAbs(-5), convOrd(353), convAbs(12), convOrd(349));
    w.fillRect(convAbs(-12), convOrd(336), convAbs(6), convOrd(330));
    w.fillRect(convAbs(7), convOrd(342), convAbs(12), convOrd(330));
    //Pause avant d'afficher le symbole du gagnant de la manche
    w.setColor("orange");
    w.sleep(2);
    w.fillRect(convAbs(-42), convOrd(319), convAbs(42), convOrd(401));

}

    /**
     *  Dessine l'étoile de Mario, pour l'afficher quand
     *  Joueur 2 gagne
     */

void etoile(DrawingWindow &w) {
    //intérieur de l'étoile
    w.setColor("yellow");
    w.fillRect(convAbs(-35), convOrd(380), convAbs(35), convOrd(365));
    w.fillRect(convAbs(-10), convOrd(395), convAbs(10), convOrd(380));
    w.fillRect(convAbs(-25), convOrd(365), convAbs(25), convOrd(335));
    w.fillRect(convAbs(15), convOrd(340), convAbs(35), convOrd(325));
    w.fillRect(convAbs(-35), convOrd(340), convAbs(-15), convOrd(325));
    //contours et yeux
    w.setColor("black");
    w.fillRect(convAbs(-5), convOrd(400), convAbs(5), convOrd(395));
    w.fillRect(convAbs(-10), convOrd(395), convAbs(-5), convOrd(385));
    w.fillRect(convAbs(5), convOrd(395), convAbs(10), convOrd(385));
    w.fillRect(convAbs(-15), convOrd(385), convAbs(-10), convOrd(375));
    w.fillRect(convAbs(10), convOrd(385), convAbs(15), convOrd(375));
    w.fillRect(convAbs(-35), convOrd(380), convAbs(-15), convOrd(375));
    w.fillRect(convAbs(15), convOrd(380), convAbs(35), convOrd(375));
    w.fillRect(convAbs(-40), convOrd(380), convAbs(-35), convOrd(370));
    w.fillRect(convAbs(35), convOrd(380), convAbs(40), convOrd(370));
    w.fillRect(convAbs(-35), convOrd(370), convAbs(-30), convOrd(365));
    w.fillRect(convAbs(30), convOrd(370), convAbs(35), convOrd(365));
    w.fillRect(convAbs(-30), convOrd(365), convAbs(-25), convOrd(360));
    w.fillRect(convAbs(-25), convOrd(360), convAbs(-20), convOrd(350));
    w.fillRect(convAbs(-30), convOrd(350), convAbs(-25), convOrd(340));
    w.fillRect(convAbs(-35), convOrd(340), convAbs(-30), convOrd(330));
    w.fillRect(convAbs(-40), convOrd(330), convAbs(-35), convOrd(320));
    w.fillRect(convAbs(-35), convOrd(325), convAbs(-25), convOrd(320));
    w.fillRect(convAbs(-25), convOrd(330), convAbs(-15), convOrd(325));
    w.fillRect(convAbs(-15), convOrd(335), convAbs(-5), convOrd(330));
    w.fillRect(convAbs(-5), convOrd(340), convAbs(5), convOrd(335));
    w.fillRect(convAbs(5), convOrd(335), convAbs(15), convOrd(330));
    w.fillRect(convAbs(15), convOrd(330), convAbs(25), convOrd(325));
    w.fillRect(convAbs(25), convOrd(325), convAbs(35), convOrd(320));
    w.fillRect(convAbs(35), convOrd(330), convAbs(40), convOrd(320));
    w.fillRect(convAbs(30), convOrd(340), convAbs(35), convOrd(330));
    w.fillRect(convAbs(25), convOrd(350), convAbs(30), convOrd(340));
    w.fillRect(convAbs(20), convOrd(360), convAbs(25), convOrd(350));
    w.fillRect(convAbs(25), convOrd(365), convAbs(30), convOrd(360));
    w.fillRect(convAbs(-10), convOrd(370), convAbs(-5), convOrd(355));
    w.fillRect(convAbs(5), convOrd(370), convAbs(10), convOrd(355));
}

    /**
     *  Dessine la flamme de Bowser, pour l'afficher quand
     *  Joueur 1 gagne
     */

void flamme(DrawingWindow &w) {
    //Partie intérieure jaune
    w.setColor("yellow");
    w.fillRect(convAbs(-30), convOrd(390), convAbs(30), convOrd(335));
    //contours oranges
    w.setColor("orange");
    w.fillRect(convAbs(-15), convOrd(395), convAbs(15), convOrd(390));
    w.fillRect(convAbs(-5), convOrd(335), convAbs(5), convOrd(330));
    w.fillRect(convAbs(-10), convOrd(340), convAbs(-5), convOrd(335));
    w.fillRect(convAbs(5), convOrd(340), convAbs(10), convOrd(335));
    w.fillRect(convAbs(-15), convOrd(350), convAbs(-10), convOrd(340));
    w.fillRect(convAbs(10), convOrd(350), convAbs(15), convOrd(340));
    w.fillRect(convAbs(-35), convOrd(375), convAbs(-30), convOrd(345));
    w.fillRect(convAbs(30), convOrd(375), convAbs(35), convOrd(345));
    w.fillRect(convAbs(-30), convOrd(345), convAbs(-25), convOrd(335));
    w.fillRect(convAbs(-20), convOrd(345), convAbs(-15), convOrd(335));
    w.fillRect(convAbs(-25), convOrd(340), convAbs(-20), convOrd(330));
    w.fillRect(convAbs(15), convOrd(345), convAbs(20), convOrd(335));
    w.fillRect(convAbs(25), convOrd(345), convAbs(30), convOrd(335));
    w.fillRect(convAbs(20), convOrd(340), convAbs(25), convOrd(330));
    w.fillRect(convAbs(-30), convOrd(385), convAbs(-25), convOrd(375));
    w.fillRect(convAbs(-25), convOrd(390), convAbs(-15), convOrd(385));
    w.fillRect(convAbs(15), convOrd(390), convAbs(25), convOrd(385));
    w.fillRect(convAbs(25), convOrd(385), convAbs(30), convOrd(375));
    //Partie la plus extérieure rouge
    w.setColor("red");
    w.fillRect(convAbs(-15), convOrd(400), convAbs(15), convOrd(395));
    w.fillRect(convAbs(15), convOrd(395), convAbs(25), convOrd(390));
    w.fillRect(convAbs(25), convOrd(390), convAbs(30), convOrd(385));
    w.fillRect(convAbs(-25), convOrd(395), convAbs(-15), convOrd(390));
    w.fillRect(convAbs(-30), convOrd(390), convAbs(-25), convOrd(385));
    w.fillRect(convAbs(-15), convOrd(340), convAbs(-10), convOrd(335));
    w.fillRect(convAbs(10), convOrd(340), convAbs(15), convOrd(335));
    w.fillRect(convAbs(-35), convOrd(385), convAbs(-30), convOrd(375));
    w.fillRect(convAbs(30), convOrd(385), convAbs(35), convOrd(375));
    w.fillRect(convAbs(-40), convOrd(375), convAbs(-35), convOrd(340));
    w.fillRect(convAbs(35), convOrd(375), convAbs(40), convOrd(340));
    w.fillRect(convAbs(-35), convOrd(345), convAbs(-30), convOrd(335));
    w.fillRect(convAbs(-35), convOrd(335), convAbs(-25), convOrd(330));
    w.fillRect(convAbs(-30), convOrd(330), convAbs(-20), convOrd(325));
    w.fillRect(convAbs(-20), convOrd(335), convAbs(-10), convOrd(330));
    w.fillRect(convAbs(-10), convOrd(335), convAbs(-5), convOrd(325));
    w.fillRect(convAbs(-5), convOrd(330), convAbs(5), convOrd(320));
    w.fillRect(convAbs(5), convOrd(335), convAbs(10), convOrd(325));
    w.fillRect(convAbs(10), convOrd(335), convAbs(20), convOrd(330));
    w.fillRect(convAbs(20), convOrd(330), convAbs(30), convOrd(325));
    w.fillRect(convAbs(25), convOrd(335), convAbs(35), convOrd(330));
    w.fillRect(convAbs(30), convOrd(345), convAbs(35), convOrd(335));
    //Yeux
    w.setColor("black");
    w.fillRect(convAbs(-15), convOrd(385), convAbs(-10), convOrd(360));
    w.fillRect(convAbs(10), convOrd(385), convAbs(15), convOrd(360));
    w.fillRect(convAbs(5), convOrd(380), convAbs(20), convOrd(365));
    w.fillRect(convAbs(-20), convOrd(380), convAbs(-5), convOrd(365));
}

    /**
     *  Reçoit la force du vent, la dessine dans la colline, dessine une
     *  flèche la représentant de façon relative à sa grandeur et la
     *  renvoie à la fonction mère.
     */

float setUpVent(DrawingWindow &w) {
    float vent = ventRand();
    int vG = vent/2;
    vG = abs(vG);
    stringstream messageVent;
    messageVent << "vent : " << (vent/VENT_MAX)*100 << "%";
    w.setColor("black");
    w.drawText(convAbs(0), convOrd(-15), messageVent.str(), Qt::AlignCenter);
    //Partie horizontale de la flèche sur 30+vent pixels
    w.drawLine(convAbs(-10-4*vG), convOrd(30), convAbs(10+3*vG), convOrd(30));
    //Vent vers la gauche
    if (vent < 0) {
        w.drawLine(convAbs(-10-4*vG), convOrd(30),
                   convAbs(5-10-4*vG), convOrd(35));
        w.drawLine(convAbs(-10-4*vG), convOrd(30), 
                   convAbs(5-10-4*vG), convOrd(25));
    }
    else {
        w.drawLine(convAbs(10+4*vG), convOrd(30),
                   convAbs(-5+10+4*vG), convOrd(35));
        w.drawLine(convAbs(10+4*vG), convOrd(30),
                   convAbs(-5+10+4*vG), convOrd(25));
    }
    return vent;
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
    *  Modélise un tour en faisant en sorte qu'à chaque lancer, les
    *  collisions, les dessins des projectiles, l'effacement des projectiles
    *  et les nouvelles positions soient prises en compte.
    */

int playerMove(int player, float ventColHColL[], DrawingWindow &w) {
    //la matrice preCoul garde en mémoire les couleurs existantes avant de
    //les écraser par l'affichage du projectile. De cette façon, on peut
    //les redessiner fidèlement afin d'effacer ce dernier.
    int angle, force, collision = 0, preCoul[16][13];
    float coord [2], vitesse [2];
    prompt(angle, force);
    //1.7 est une constante de "vraissemblabilité" qui aide à passer d'un
    //système de mesures réel à un système de mesure basé sur le pixel
    vitesse[0] = 1.7*force*cos(M_PI*angle/180);
    vitesse[1] = 1.7*force*sin(M_PI*angle/180);
    coordInit(player, coord);
    while(collision == 0 || collision == -1) {
        collision = checkCollision(coord, ventColHColL);
        bufferCouleurs(preCoul, coord, ventColHColL, w);
        tortue(w, coord);
        w.msleep(15);
        effaceTortue(preCoul, coord, w);
        nPosition(coord, vitesse, ventColHColL[0], player);
    } 
    return collision;
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
        //On ne passera dans cette condition que si l'on a entré une
        //force invalide (c'est à dire après une première tentative
        //d'entrée de donnée infructueuse)
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
     *  Met au point les coordonnées initiales des projectiles selon le
     *  numéro du joueur, pour savoir à quel chateau il correspond.
     */

void coordInit(int p, float coord[]) {
    coord[1] = 41;
    if (p == 1) {
        coord[0] = -F_LARG/2+38; //à droite de l'abs du drapeau de Bowser
    }
    else {
        coord[0] = F_LARG/2-55; //à gauche de l'abs du drapeau de Mario
    }
}

   /**
    *   renvoie un code collision selon le type de collision rencontré
    *   Codes collision :
    *   Pas de collision : _____________________    0
    *   collision chateau p2 : _________________    1 
    *   collision chateau p1 : _________________    2
    *   collision sol : ________________________    3
    *   collision colline : ____________________    4
    *   hors de la fenêtre côté verticale :_____   -1
    *   hors de la fenêtre côté horizontale :___   -2
    *   hors de la fenêtre horizontale et
    *       collision avec le sol (cas rare) :__   -3
    */

int checkCollision(float p[], float vCC[]) { 
    int col = 0;
    if (p[0] >= 260 && p[0] <= 300 && p[1] <= 40) //chateau droite
        col = 1;
    else if (p[0] >= -300 && p[0] <= -260 && p[1] <= 40) //chateau gauche
        col = 2;
    else if (p[1] <= 0) //sol
        col = 3;
    else if ((vCC[1]*(1-(2*p[0]/vCC[2])*(2*p[0]/vCC[2]))) > p[1]) //colline
        col = 4;
    else if (p[1] <= -30 || p[1] >= F_HAUT-30)
        col = -1;

    if (p[0] >= F_LARG/2 || p[0] <= -F_LARG/2) { //sortie de fenêtre
        if (col == 0)
            col = -2;
        else if (col == 3)
            col = -3;
    }
    return col;
}

    /**
     *  Met en mémoire les couleurs existantes avant/sous le dessin d'une
     *  projectile pour pouvoir l'effacer par la suite de façon propre.
     */

void bufferCouleurs(int buffer[16][13], float coord[],
                    float ventColHColL[], DrawingWindow &w) {
    float coordTortue[2];
    for (int x = 0; x < 16; x += 1) { //Pos min et max de x de la tortue
        for(int y = 0; y < 13; y += 1) {
            coordTortue[0] = coord[0] + x - 7;
            coordTortue[1] = coord[1] + y;
            if(checkCollision(coordTortue, ventColHColL) >= 0)
                buffer[x][y] = w.getPointColor(convAbs(coordTortue[0]),
                                                convOrd(coordTortue[1]));
        }
    }
}

    /**
     *  Dessine une carapace de tortue (projectile) au dessus et de part
     *  et d'autre d'un point fourni en paramètres
     */

void tortue(DrawingWindow &w, float point[]) {
    float x = point[0], y = point[1];
    //Le milieu de la carapace
    w.setColor("darkgreen");
    w.fillRect(convAbs(x-3), convOrd(y+11), convAbs(x+4), convOrd(y+9));
    w.fillRect(convAbs(x-6), convOrd(y+8),convAbs(x+7), convOrd(y+4));
    //Les cotes de la carapace
    w.setColor("green");
    w.fillRect(convAbs(x-2), convOrd(y+9), convAbs(x+3), convOrd(y+7));
    //le bas de la carapace
    w.setColor("White");
    w.fillRect(convAbs(x-6), convOrd(y+4), convAbs(x-2), convOrd(y+2));
    w.fillRect(convAbs(x+3), convOrd(y+4), convAbs(x+7), convOrd(y+2));
    w.fillRect(convAbs(x-1), convOrd(y+2), convAbs(x+2), convOrd(y+1));
    //les contours
    w.setColor("black");
    w.drawLine(convAbs(x-1), convOrd(y), convAbs(x+2), convOrd(y));
    w.drawLine(convAbs(x-3), convOrd(y+1), convAbs(x-2), convOrd(y+1));
    w.drawLine(convAbs(x+3), convOrd(y+1), convAbs(x+4), convOrd(y+1));
    w.drawLine(convAbs(x-6), convOrd(y+2), convAbs(x-3), convOrd(y+2));
    w.drawLine(convAbs(x+4), convOrd(y+2), convAbs(x+7), convOrd(y+2));
    w.drawPoint(convAbs(x-6), convOrd(y+3));
    w.drawPoint(convAbs(x+7), convOrd(y+3));
    w.drawLine(convAbs(x-7), convOrd(y+7), convAbs(x-7), convOrd(y+3));
    w.drawLine(convAbs(x+8), convOrd(y+7), convAbs(x+8), convOrd(y+3));
    w.drawLine(convAbs(x-1), convOrd(y+3), convAbs(x+2), convOrd(y+3));
    w.drawLine(convAbs(x-4), convOrd(y+4), convAbs(x-2), convOrd(y+4));
    w.drawLine(convAbs(x+3), convOrd(y+4), convAbs(x+5), convOrd(y+4));
    w.drawPoint(convAbs(x-5), convOrd(y+5));
    w.drawPoint(convAbs(x+6), convOrd(y+5));
    w.drawPoint(convAbs(x-2), convOrd(y+5));
    w.drawPoint(convAbs(x+3), convOrd(y+5));
    w.drawLine(convAbs(x-6), convOrd(y+9), convAbs(x-6), convOrd(y+7));
    w.drawLine(convAbs(x+7), convOrd(y+9), convAbs(x+7), convOrd(y+7));
    w.drawRect(convAbs(x-5), convOrd(y+10), convAbs(x-4), convOrd(y+9));
    w.drawRect(convAbs(x+5), convOrd(y+10), convAbs(x+6), convOrd(y+9));
    w.drawLine(convAbs(x-2), convOrd(y+12), convAbs(x+3), convOrd(y+12));
    w.drawLine(convAbs(x-3), convOrd(y+11), convAbs(x-2), convOrd(y+11));
    w.drawLine(convAbs(x+3), convOrd(y+11), convAbs(x+4), convOrd(y+11));
    w.drawLine(convAbs(x-6), convOrd(y+5), convAbs(x-1), convOrd(y+10));
    w.drawLine(convAbs(x+2), convOrd(y+10), convAbs(x+7), convOrd(y+5));
    w.drawLine(convAbs(x-2), convOrd(y+7), convAbs(x-1), convOrd(y+6));
    w.drawLine(convAbs(x+3), convOrd(y+7), convAbs(x+2), convOrd(y+6));
    w.drawLine(convAbs(x), convOrd(y+6), convAbs(x+1), convOrd(y+6));
    w.drawLine(convAbs(x), convOrd(y+10), convAbs(x+1), convOrd(y+10));
}

    /**
     *  Efface un projectile précédement dessiné.
     */

void effaceTortue(int couleurs[16][13], float coord[], DrawingWindow &w) {
    float coordTortue[2];
    for (int x = 0; x < 16; x += 1) { //Pos min et max de x de la tortue
        for(int y = 0; y < 13; y += 1) {
        //Le point déterminant du projectile est au milieu, 7 pixels à droite
        //du point le plus à gauche balayé par la boucle.
            coordTortue[0] = coord[0] + x - 7;
            coordTortue[1] = coord[1] + y;
            w.setColor(couleurs[x][y]);
            w.drawPoint(convAbs(coordTortue[0]), convOrd(coordTortue[1]));
        }
    }
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
    //vr étant calculé au début, on peut se permettre de faire le calcul 
    //des nouvelles accélerations directement dans le calcul des nouvelles
    //vitesses
    v[0] += (REFRESH * (-KFROT * (vr) * (v[0] - vVent)));
    v[1] += (REFRESH * (-KFROT * (vr) * v[1] - GRAV));
}

    /**
     *  Renvoie une abscisse utilisable par la bibliothèque graphique à
     *  partir d'une abscisse d'un système dans lequel l'origine serait
     *  au milieu de la colline, au niveau du sol.
     */

int convAbs(float abs) {
    return abs + F_LARG/2;
}

    /**
     *  Renvoie une ordonnée utilisable par la bibliothèque graphique à
     *  partir d'une ordonnée d'un système dans lequel l'origine serait
     *  au milieu de la colline, au niveau du sol
     */

int convOrd(float ord) {
    return F_HAUT - 30 - ord;
}
