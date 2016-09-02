
#include <cassert>
#include <math.h>
#include <stdio.h>

#include "draw.h"        // pour dessiner du point de vue d'une camera
#include "viewer.h"


Viewer* Viewer::s_singleton = NULL;

Viewer::Viewer() : program(0), b_draw_grid(true)
{
    s_singleton=this;
}


void Viewer::help()
{
    printf("\th: help\n");
    printf("\ta: (des)active l'affichage de l'axe\n");
    printf("\tg: (des)active l'affichage de la grille\n");
}

int Viewer::init()
{
    //system("pwd");
    // Creer une camera par defaut, elle est placee en 0, 0, 5 et regarde les objets situes en 0, 0, 0
    //camera= make_orbiter();

    // etat par defaut openGL
    glClearColor(0.5f, 0.5f, 0.9f, 1);
    glClearDepthf(1);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);


    init_axe();
    init_grid();
    init_cube();
    init_quad();

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


void Viewer::init_cube()
{
    static float pt[8][3] = { {-1,-1,-1}, {1,-1,-1}, {1,-1,1}, {-1,-1,1}, {-1,1,-1}, {1,1,-1}, {1,1,1}, {-1,1,1} };
    static int f[6][4] = { {0,1,2,3}, {5,4,7,6}, {1,5,6,2}, {0,3,7,4}, {3,2,6,7}, {0,4,5,1} };
    static float n[6][3] = { {0,-1,0}, {0,1,0}, {1,0,0}, {-1,0,0}, {0,0,1}, {0,0,-1} };
    static float uv[4][2] = { {0,0}, {1,0}, {1,1}, {0,1} };
    int i,j;

    cube = create_mesh(GL_TRIANGLES);
    vertex_color(cube, make_color(1, 0, 1));

    cube_texture = read_texture(0, "data/debug2x2red.png");

    for (i=0;i<6;i++)
    {
        vertex_normal(cube, vec3(n[i][0], n[i][1], n[i][2] ) );

        vertex_texcoord(cube, 0,0 );
        push_vertex(cube, pt[ f[i][0] ][0], pt[ f[i][0] ][1], pt[ f[i][0] ][2] );

        vertex_texcoord(cube, 1,0);
        push_vertex(cube, pt[ f[i][1] ][0], pt[ f[i][1] ][1], pt[ f[i][1] ][2] );

        vertex_texcoord(cube, 0,1);
        push_vertex(cube, pt[ f[i][3] ][0], pt[ f[i][3] ][1], pt[ f[i][3] ][2] );


        vertex_texcoord(cube, 1,0);
        push_vertex(cube, pt[ f[i][1] ][0], pt[ f[i][1] ][1], pt[ f[i][1] ][2] );

        vertex_texcoord(cube, 1,1);
        push_vertex(cube, pt[ f[i][2] ][0], pt[ f[i][2] ][1], pt[ f[i][2] ][2] );

        vertex_texcoord(cube, 0,1);
        push_vertex(cube, pt[ f[i][3] ][0], pt[ f[i][3] ][1], pt[ f[i][3] ][2] );
    }
}



void Viewer::init_quad()
{
    quad = create_mesh(GL_TRIANGLES);
    vertex_color(quad, make_color(1, 1, 1));

    quad_texture = read_texture(0, "data/papillon.jpg");

    vertex_normal(quad, vec3( 0, 0, -1 ) );

    vertex_texcoord(quad, 0,0 );
    push_vertex(quad, -1, -1, 0 );

    vertex_texcoord(quad, 1,0);
    push_vertex(quad,  1, -1, 0 );

    vertex_texcoord(quad, 0,1);
    push_vertex(quad,  -1, 1, 0 );


    vertex_texcoord(quad, 1,0);
    push_vertex(quad,  1, -1, 0 );

    vertex_texcoord(quad, 1,1);
    push_vertex(quad,  1,  1, 0 );

    vertex_texcoord(quad, 0,1);
    push_vertex(quad, -1,  1, 0 );
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
    if((mb & SDL_BUTTON(1)) &&  (mb& SDL_BUTTON(3)))                 // le bouton du milieu est enfonce
        orbiter_translation(camera, (float) mx / (float) window_width(), (float) my / (float) window_height());         // deplace le point de rotation
    else if(mb & SDL_BUTTON(1))                      // le bouton gauche est enfonce
        orbiter_rotation(camera, mx, my);       // tourne autour de l'objet
    else if(mb & SDL_BUTTON(3))                 // le bouton droit est enfonce
        orbiter_move(camera, mx);               // approche / eloigne l'objet

    if (key_state(SDLK_h)) help();
    if (key_state(SDLK_g)) { b_draw_grid = !b_draw_grid; clear_key_state(SDLK_g); }
    if (key_state(SDLK_a)) { b_draw_axe = !b_draw_axe; clear_key_state(SDLK_a); }
    if (key_state(SDLK_DOWN)) { orbiter_move(camera, -1); }
    if (key_state(SDLK_UP)) { orbiter_move(camera, 1); }


    // on dessine l'objet du point de vue de la camera
    if (b_draw_grid) ::draw(grid, camera);
    if (b_draw_axe) ::draw(axe, camera);

    Transform Tc= make_translation( -3, 5, 0 );
    ::draw(cube, Tc, camera, cube_texture);

    Transform Tq= make_translation( 3, 5, 0 );
    ::draw(quad, Tq, camera, quad_texture);


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
