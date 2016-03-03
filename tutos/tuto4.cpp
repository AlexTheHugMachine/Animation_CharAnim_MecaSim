
//! \file tuto4.cpp utiliser mesh pour dessiner un triangle du point de vue d'une camera

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
    triangle= create_mesh(GL_TRIANGLES);
    
    vertex_color(triangle, make_color(1, 0, 0));
    push_vertex(triangle, -0.5, -0.5, 0);
    
    vertex_color(triangle, make_color(0, 1, 0));
    push_vertex(triangle,  0.5,  0.5, 0);
    
    vertex_color(triangle, make_color(0, 0, 1));
    push_vertex(triangle, -0.5,  0.5, 0);
    
    // etape 2 : creer une camera par defaut, elle est placee en 0, 0, 5 et regarde les objets situes en 0, 0, 0
    camera= make_orbiter();
    
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
    
// on peut aussi controler la position de la camera
/*
    float avance= 0;
    if(key_state('j'))
        avance= 1;
    if(key_state('k'))
        avance= -1;
    
    orbiter_move(camera, avance);
    draw(triangle, camera);
    
    return 1;    
 */
 // et ses rotations, cf orbiter_rotation( ), orbiter_translation( )...
 // on peut aussi recuperer les mouvements de la souris et l'utiliser pour controler la camera... cf SDL_GetRelativeMouseState( )
}

int quit( )
{
    // etape 3 : detruire la description du triangle
    release_mesh(triangle);
    return 0;   // ras, pas d'erreur
}

int main( int argc, char **argv )
{
    Window w= create_window(1024, 640);
    if(w == NULL) 
        return 1;       // erreur lors de la creation de la fenetre ou de l'init de sdl2
    
    Context c= create_context(w);
    if(c == NULL) 
        return 1;       // erreur lors de la creation du contexte opengl
    
    // creation des objets opengl
    if(init() < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }
    
    // affichage de l'application, tant que la fenetre n'est pas fermee. ou que draw() ne renvoie pas 0
    run(w);

    // nettoyage
    quit();
    release_context(c);
    release_window(w);
    return 0;
}

