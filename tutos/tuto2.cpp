
//! \file tuto2.cpp utilisation de mesh pour decrire les sommets d'un ou plusieurs triangles et les dessiner.

#include "window.h"
#include "mesh.h"

/* par defaut, openGL dessine les objets qui se trouvent entre -1 et 1 sur x, y et z.
 */

Mesh triangle;


int init( )
{
    // etape 1 : decrire le triangle, les coordonnees des 3 sommets.
    
    // creer un mesh pour stocker les coordonnees des sommets du triangle
    triangle= create_mesh(GL_TRIANGLES);
    
    // choisir une couleur pour les sommets, rouge + vert = jaune
    vertex_color(triangle, make_color(1, 1, 0));
    
    // donner les positions des 3 sommets, entre -1 1 sur x, y, z
    push_vertex(triangle, -0.5, -0.5, 0);
    push_vertex(triangle,  0.5,  0.5, 0);
    push_vertex(triangle, -0.5,  0.5, 0);
    
    return 0;   // ras, pas d'erreur
    
// on peut aussi donner des couleurs differentes aux sommets du triangle :
/*
    vertex_color(triangle, make_color(1, 0, 0));
    push_vertex(triangle, -0.5, -0.5, 0);
    
    vertex_color(triangle, make_color(0, 1, 0));
    push_vertex(triangle,  0.5,  0.5, 0);
    
    vertex_color(triangle, make_color(0, 0, 1));
    push_vertex(triangle, -0.5,  0.5, 0);
    
    return 0;   // ras, pas d'erreur
 */
}

int draw( )
{
    // etape 2 : dessiner l'objet avec opengl
    
    // on commence par effacer la fenetre avant de dessiner quelquechose
    glClear(GL_COLOR_BUFFER_BIT);
    
    // on dessine le triangle sans changer les transformations.
    draw(triangle, make_identity(), make_identity(), make_identity());
    
    return 1;   // on continue, renvoyer 0 pour sortir de l'application
}

int quit( )
{
    // etape 3 : detruire la description du triangle
    release_mesh(triangle);
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
