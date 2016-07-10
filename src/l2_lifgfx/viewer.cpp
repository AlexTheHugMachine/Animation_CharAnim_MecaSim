
#include <cassert>

#include "Viewer.h"

#include <stdio.h>
#include "draw.h"        // pour dessiner du point de vue d'une camera


Viewer* Viewer::s_singleton = NULL;

Viewer::Viewer() : program(0), texture(0), b_draw_grid(true)
{
    s_singleton=this;
}


void Viewer::help()
{
    printf("\th: help\n");
}

int Viewer::init()
{
    // Creer une camera par defaut, elle est placee en 0, 0, 5 et regarde les objets situes en 0, 0, 0
    camera= make_orbiter();

    init_axe();
    init_cube();
    init_grid();

    // etat par defaut openGL
    glClearColor(0.5f, 0.5f, 0.9f, 1);
    glClearDepthf(1);
    // glViewport(0, 0, window_width(), window_height()) // deja fait dans run( )

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

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
    static float pt[8][3] = { {0,0,0}, {1,0,0}, {1,0,1}, {0,0,1}, {0,1,0}, {1,1,0}, {1,1,1}, {0,1,1} };
    static int f[6][4] = { {0,1,2,3}, {5,4,7,6}, {1,5,6,2}, {0,3,7,4}, {3,2,6,7}, {0,4,5,1} };
    static float n[6][3] = { {0,-1,0}, {0,1,0}, {1,0,0}, {-1,0,0}, {0,0,1}, {0,0,-1} };
    static float uv[4][2] = { {0,0}, {1,0}, {1,1}, {0,1} };
    int i,j;

    cube = create_mesh(GL_TRIANGLES);
    vertex_color(cube, make_color(1, 0, 0));

    for (i=0;i<6;i++)
    {
        //glNormal3f( n[ i ][0], n[ i ][1], n[ i ][2] );
        push_vertex(cube, pt[ f[i][0] ][0], pt[ f[i][0] ][1], pt[ f[i][0] ][2] );
        push_vertex(cube, pt[ f[i][1] ][0], pt[ f[i][1] ][1], pt[ f[i][1] ][2] );
        push_vertex(cube, pt[ f[i][3] ][0], pt[ f[i][3] ][1], pt[ f[i][3] ][2] );

        push_vertex(cube, pt[ f[i][1] ][0], pt[ f[i][1] ][1], pt[ f[i][1] ][2] );
        push_vertex(cube, pt[ f[i][2] ][0], pt[ f[i][2] ][1], pt[ f[i][2] ][2] );
        push_vertex(cube, pt[ f[i][3] ][0], pt[ f[i][3] ][1], pt[ f[i][3] ][2] );
    }
}

void Viewer::init_grid()
{
    grid = create_mesh(GL_LINES);

    vertex_color(grid, make_color(1, 1, 1));
    int i,j;
    for(i=-5;i<=5;++i)
        for(j=-5;j<=5;++j)
        {
            push_vertex(grid, -5, 0, j);
            push_vertex(grid, 5, 0,  j);

            push_vertex(grid, i, 0, -5);
            push_vertex(grid, i, 0, 5);

        }
}


int Viewer::quit( )
{
    return 0;
}


int Viewer::draw( )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // recupere les mouvements de la souris pour deplacer la camera, cf tutos/tuto6.cpp
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
    // deplace la camera
    if(mb & SDL_BUTTON(1))                      // le bouton gauche est enfonce
        orbiter_rotation(camera, mx, my);       // tourne autour de l'objet
    else if(mb & SDL_BUTTON(3))                 // le bouton droit est enfonce
        orbiter_move(camera, mx);               // approche / eloigne l'objet
    else if(mb & SDL_BUTTON(2))                 // le bouton du milieu est enfonce
        orbiter_translation(camera, (float) mx / (float) window_width(), (float) my / (float) window_height());         // deplace le point de rotation

    if (key_state(SDLK_h)) help();
    if (key_state(SDLK_g)) { b_draw_grid = !b_draw_grid; clear_key_state(SDLK_g); }
    if (key_state(SDLK_a)) { b_draw_axe = !b_draw_axe; clear_key_state(SDLK_a); }
    if (key_state(SDLK_DOWN)) { orbiter_move(camera, -1); }
    if (key_state(SDLK_UP)) { orbiter_move(camera, 1); }


    // on dessine l'objet du point de vue de la camera
    if (b_draw_grid) ::draw(grid, camera);
    if (b_draw_axe) ::draw(axe, camera);

    Transform T= make_translation( 1, 1, 0 );
    ::draw(cube, T, camera);
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
