
#include <cassert>

#include "Viewer.h"

#include <stdio.h>
#include "draw.h"        // pour dessiner du point de vue d'une camera


Viewer* Viewer::s_singleton = NULL;

Viewer::Viewer() : program(0), texture(0)
{
    s_singleton=this;
}


int Viewer::init()
{
    // Creer une camera par defaut, elle est placee en 0, 0, 5 et regarde les objets situes en 0, 0, 0
    camera= make_orbiter();

    init_axe();
    init_cube();

    // etat par defaut openGL
    glClearColor(0.5f, 0.5f, 0.9f, 1);

    return 0;
}


void Viewer::init_axe()
{
    axe = create_mesh(GL_LINES);
    vertex_color(axe, make_color(1, 0, 0));
    push_vertex( axe,  0,  0, 0);
    push_vertex( axe,  1,  0, 0);

    vertex_color(axe, make_color(0, 1, 0));
    push_vertex( axe,  0,  0, 0);
    push_vertex( axe,  0,  1, 0);

    vertex_color(axe, make_color( 0, 0, 1));
    push_vertex( axe,  0,  0, 0);
    push_vertex( axe,  0,  0, 1);
}


void Viewer::init_cube()
{
    cube = create_mesh(GL_QUADS);

    vertex_color(cube, make_color(1, 0, 0));
    push_vertex(cube, -0.5, -0.5, 0);

    vertex_color(cube, make_color(0, 1, 0));
    push_vertex(cube,  -0.5,  0.5, 0);

    vertex_color(cube, make_color(0, 0, 1));
    push_vertex(cube, 0.5,  0.5, 0);

    vertex_color(cube, make_color(0, 1, 1));
    push_vertex(cube, 0.5,  -0.5, 0);
}


int Viewer::quit( )
{
    return 0;
}


int Viewer::draw( )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // recupere les mouvements de la souris pour deplacer la camera, cf tutos/tuto6.cpp
    // recupere les mouvements de la souris, utilise directement SDL2
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);

    // deplace la camera
    if(mb & SDL_BUTTON(1))                      // le bouton gauche est enfonce
        orbiter_rotation(camera, mx, my);       // tourne autour de l'objet
    else if(mb & SDL_BUTTON(3))                 // le bouton droit est enfonce
        orbiter_move(camera, mx);               // approche / eloigne l'objet
    else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
        orbiter_translation(camera, (float) mx / (float) window_width(), (float) my / (float) window_height());         // deplace le point de rotation


    // on dessine l'objet du point de vue de la camera
    ::draw(cube, camera);
    ::draw(axe, camera);

    //Transform R= make_rotationZ( 45 );
    //Transform T= make_translation( 1, 0, 0 );
    //::draw(cube, T, camera);
    //::draw(cube, camera);

    return 1;
}



int main( int argc, char **argv )
{
    // etape 1 : creer la fenetre
    Window window= create_window(1024, 640);
    if(window == NULL)  return 1;       // erreur lors de la creation de la fenetre ou de l'init de sdl2

    // etape 2 : creer un contexte opengl pour pouvoir dessiner
    Context context= create_context(window);
    if(context == NULL)  return 1;       // erreur lors de la creation du contexte opengl


    Viewer v;

    // etape 3 : creation des objets
    if(v.init() < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }

    // etape 4 : affichage de l'application, tant que la fenetre n'est pas fermee. ou que draw() ne renvoie pas 0
    run(window, Viewer::singleton_draw);

    // etape 5 : nettoyage
    v.quit();

    release_context(context);
    release_window(window);
    return 0;
}
