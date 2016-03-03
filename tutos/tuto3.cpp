
//! \file tuto3.cpp utiliser mesh pour dessiner un triangle avec une rotation

#include "window.h"
#include "mesh.h"

/* 
    par defaut, openGL dessine les objets qui se trouvent entre -1 et 1 sur x, y et z.
 */

Mesh triangle;


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
    
    return 0;   // ras, pas d'erreur
}

int draw( )
{
    // etape 2 : dessiner l'objet avec opengl
    
    // on commence par effacer la fenetre avant de dessiner quelquechose
    glClear(GL_COLOR_BUFFER_BIT);
    
    // on dessine le triangle avec une transformation : rotation autour de Z
    /* 
        openGL utilise un repere droit, x vers la droite, y vers le haut et z devant (vers l'observateur)
        donc une rotation autour de l'axe Z permet de faire tourner le triangle dans l'image.
     */
    Transform T= make_rotationZ( 45 );
    draw(triangle, T, make_identity(), make_identity());
    
    return 1;   // on continue, renvoyer 0 pour sortir de l'application
    
// on peut aussi controler la rotation du triangle au clavier...
/*
    static float angle= 0;      // il faudrait declarer angle comme variable globale...
    if(key_state('j'))
        angle= angle + 1;
    if(key_state('k'))
        angle= angle - 1;
        
    Transform T= make_rotationZ( angle );
    draw(triangle, T, make_identity(), make_identity());
    
    return 1;    
 */
// utiliser une autre transformation : make_scale(), make_translation(), etc.
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

