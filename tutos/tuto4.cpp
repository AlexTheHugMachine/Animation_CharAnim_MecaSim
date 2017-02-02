
//! \file tuto4.cpp utiliser mesh pour dessiner un triangle du point de vue d'une camera

#include <stdio.h>
#include "window.h"
#include "mesh.h"

#include "orbiter.h"    // camera

#include "draw.h"        // pour dessiner du point de vue d'une camera

/*
    par defaut, openGL dessine les objets qui se trouvent entre -1 et 1 sur x, y et z.
 */

Mesh triangle;
Orbiter camera;

int init( )
{
    // etape 1 : decrire le triangle, les coordonnees des 3 sommets.

    // creer un mesh pour stocker les coordonnees des sommets du triangle
    triangle= Mesh(GL_TRIANGLES);
    
    triangle.color(Color(1, 0, 0));
    triangle.vertex(-0.5, -0.5, 0);

    triangle.color(Color(0, 1, 0));
    triangle.vertex(0.5,  0.5, 0);

    triangle.color(Color(0, 0, 1));
    triangle.vertex(-0.5,  0.5, 0);

    // etape 2 : creer une camera par defaut, elle est placee en 0, 0, 5 et regarde les objets situes en 0, 0, 0
    camera= Orbiter();

    return 0;   // ras, pas d'erreur
}

int draw( )
{
    // etape 2 : dessiner l'objet avec opengl

    // on commence par effacer la fenetre avant de dessiner quelquechose
    glClear(GL_COLOR_BUFFER_BIT);

    // on dessine le triangle du point de vue de la camera
    draw(triangle, camera);

    return 1;   // on continue, renvoyer 0 pour sortir de l'application

// on peut aussi controler la position de la camera, au clavier, par exemple 
/*
    float avance= 0;
    if(key_state('j'))
        avance= 1;
    if(key_state('k'))
        avance= -1;

    camera.move(avance);
    //~ camera.translation(avance / 100, 0);
    draw(triangle, camera);

    return 1;
 */
 // et ses rotations, cf Orbiter::rotation( ), Orbiter::translation( )...
 // on peut aussi recuperer les mouvements de la souris et l'utiliser pour controler la camera... cf SDL_GetRelativeMouseState( )
}

int quit( )
{
    // etape 3 : detruire la description du triangle
    triangle.release();
    return 0;   // ras, pas d'erreur
}


int main( int argc, char **argv )
{
    // etape 1 : creer la fenetre
    Window window= create_window(1024, 640);
    if(window == NULL)
        return 1;

    // etape 2 : creer un contexte opengl pour pouvoir dessiner
    Context context= create_context(window);
    if(context == NULL)
        return 1;

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
