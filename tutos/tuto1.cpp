
//! \file tuto1.cpp creation d'une application vide. affiche une fenetre vide / noire.

#include <stdio.h>
#include "window.h"

/* une application opengl est composee de plusieurs composants :
    1. une fenetre pour voir ce que l'on dessine
    2. un contexte openGL pour dessiner
    3. 3 fonctions :
        init( ) pour creer les objets que l'on veut dessiner,
        draw( ) pour les afficher / dessiner
        quit( ) pour detruire les objets openGL crees dans init( )

    ces 3 fonctions sont appelees dans le main.

    draw( ) est un peu a part, elle est appellee par la fonction run( ) qui traite les evenements clavier, souris, clicks, etc,
    ca permet de reagir a ces evenements et de modifier / deplacer / animer ce que l'on dessine.
 */

// creation des objets openGL
int init( )
{
    return 0;   // renvoyer 0 ras, pas d'erreur, sinon renvoyer -1
}

// affichage
int draw( )
{
    return 1;   // on continue, renvoyer 0 pour sortir de l'application
}

// destruction des objets openGL
int quit( )
{
    return 0;   // ras, pas d'erreur
}


int main( int argc, char **argv )
{
    // etape 1 : creer la fenetre
    Window window= create_window(1024, 640);
    if(window == NULL)
        return 1;       // erreur lors de la creation de la fenetre ou de l'init de sdl2

    // etape 2 : creer un contexte opengl pour pouvoir dessiner
    Context context= create_context(window);
    if(context == NULL)
        return 1;       // erreur lors de la creation du contexte opengl

    // etape 3 : creation des objets
    if(init() < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }

    // etape 4 : affichage de l'application, tant que la fenetre n'est pas fermee. ou que draw() ne renvoie pas 0
    run(window, draw);

    // etape 5 : nettoyage
    quit();
    release_context(context);
    release_window(window);
    return 0;
}

