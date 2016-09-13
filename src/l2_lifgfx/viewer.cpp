
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "draw.h"        // pour dessiner du point de vue d'une camera
#include "viewer.h"

using namespace std;


Viewer::Viewer() : App(1024, 640), b_draw_grid(true), b_draw_axe(true)
{
}


void Viewer::help()
{
    printf("HELP:\n");
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

    //camera.move( -50 );
    camera.lookat( Point(0,0,0), 30 );
    draw_param.light( Point(0, 20, 20), White() );

    init_axe();
    init_grid();
    init_cube();
    init_quad();

    return 0;
}


void Viewer::init_axe()
{
    axe = Mesh(GL_LINES);
    axe.color( Color(1, 0, 0));
    axe.vertex( 0,  0, 0);
    axe.vertex( 1,  0, 0);

    axe.color( Color(0, 1, 0));
    axe.vertex( 0,  0, 0);
    axe.vertex( 0,  1, 0);

    axe.color( Color( 0, 0, 1));
    axe.vertex( 0,  0, 0);
    axe.vertex( 0,  0, 1);
}


void Viewer::init_grid()
{
    grid = Mesh(GL_LINES);

    grid.color( Color(1, 1, 1));
    int i,j;
    for(i=-5;i<=5;++i)
        for(j=-5;j<=5;++j)
        {
            grid.vertex( -5, 0, j);
            grid.vertex( 5, 0,  j);

            grid.vertex( i, 0, -5);
            grid.vertex( i, 0, 5);

        }
}


void Viewer::init_cube()
{
    static float pt[8][3] = { {-1,-1,-1}, {1,-1,-1}, {1,-1,1}, {-1,-1,1}, {-1,1,-1}, {1,1,-1}, {1,1,1}, {-1,1,1} };
    static int f[6][4] = { {0,1,2,3}, {5,4,7,6}, {1,5,6,2}, {0,3,7,4}, {3,2,6,7}, {0,4,5,1} };
    static float n[6][3] = { {0,-1,0}, {0,1,0}, {1,0,0}, {-1,0,0}, {0,0,1}, {0,0,-1} };
    static float uv[4][2] = { {0,0}, {1,0}, {1,1}, {0,1} };
    int i,j;

    cube = Mesh(GL_TRIANGLES);
    cube.color( Color(1, 1, 1) );

    cube_texture = read_texture(0, "data/debug2x2red.png");

    for (i=0;i<6;i++)
    {
        cube.normal(  n[i][0], n[i][1], n[i][2] );

        cube.texcoord( 0,0 );
        cube.vertex( pt[ f[i][0] ][0], pt[ f[i][0] ][1], pt[ f[i][0] ][2] );

        cube.texcoord( 1,0);
        cube.vertex( pt[ f[i][1] ][0], pt[ f[i][1] ][1], pt[ f[i][1] ][2] );

        cube.texcoord(0,1);
        cube.vertex(pt[ f[i][3] ][0], pt[ f[i][3] ][1], pt[ f[i][3] ][2] );

        cube.texcoord(1,0);
        cube.vertex( pt[ f[i][1] ][0], pt[ f[i][1] ][1], pt[ f[i][1] ][2] );

        cube.texcoord(1,1);
        cube.vertex( pt[ f[i][2] ][0], pt[ f[i][2] ][1], pt[ f[i][2] ][2] );

        cube.texcoord(0,1);
        cube.vertex( pt[ f[i][3] ][0], pt[ f[i][3] ][1], pt[ f[i][3] ][2] );
    }
}



void Viewer::init_quad()
{
    quad = Mesh(GL_TRIANGLES);
    quad.color( Color(1, 1, 1));

    quad_texture = read_texture(0, "data/papillon.jpg");

    quad.normal(  0, 0, 1 );

    quad.texcoord(0,0 );
    quad.vertex(-1, -1, 0 );

    quad.texcoord(1,0);
    quad.vertex(  1, -1, 0 );

    quad.texcoord(0,1);
    quad.vertex( -1, 1, 0 );


    quad.texcoord(1,0);
    quad.vertex( 1, -1, 0 );

    quad.texcoord( 1,1);
    quad.vertex(  1,  1, 0 );

    quad.texcoord( 0,1);
    quad.vertex(-1,  1, 0 );
}







int Viewer::quit( )
{
    return 0;
}



int Viewer::render( )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // recupere les mouvements de la souris pour deplacer la camera, cf tutos/tuto6.cpp
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
    // deplace la camera
    if((mb & SDL_BUTTON(1)) &&  (mb& SDL_BUTTON(3)))                 // le bouton du milieu est enfonce
        camera.translation( (float) mx / (float) window_width(), (float) my / (float) window_height());         // deplace le point de rotation
    else if(mb & SDL_BUTTON(1))                      // le bouton gauche est enfonce
        camera.rotation( mx, my);       // tourne autour de l'objet
    else if(mb & SDL_BUTTON(3))                 // le bouton droit est enfonce
        camera.move( mx);               // approche / eloigne l'objet

    const float step = 0.1f;
    if (key_state(SDLK_RIGHT) && key_state(SDLK_LCTRL)) { draw_param.light( draw_param.light()+Vector(step,0,0)); }
    if (key_state(SDLK_LEFT) && key_state(SDLK_LCTRL)) { draw_param.light( draw_param.light()+Vector(-step,0,0)); }
    if (key_state(SDLK_UP) && key_state(SDLK_LCTRL)) { draw_param.light( draw_param.light()+Vector(0,step,0)); }
    if (key_state(SDLK_DOWN) && key_state(SDLK_LCTRL)) { draw_param.light( draw_param.light()+Vector(0,-step,0)); }
    if (key_state(SDLK_PAGEUP) && key_state(SDLK_LCTRL)) { draw_param.light( draw_param.light()+Vector(0,0,step)); }
    if (key_state(SDLK_PAGEDOWN) && key_state(SDLK_LCTRL)) { draw_param.light( draw_param.light()+Vector(0,0, -step)); }
    if (key_state(SDLK_DOWN) && (!key_state(SDLK_LCTRL))) { camera.move( -1); }
    if (key_state(SDLK_UP) && (!key_state(SDLK_LCTRL))) { camera.move( 1); }


    if (key_state('h')) help();
    if (key_state('g')) { b_draw_grid = !b_draw_grid; clear_key_state('g'); }
    if (key_state('a')) { b_draw_axe = !b_draw_axe; clear_key_state('a'); }


    Transform Tq= Translation( 3, 5, 0 );
    draw_param.texture(quad_texture).camera(camera).draw(quad);
    //::draw(quad, Tq, camera, quad_texture);


    // on dessine l'objet du point de vue de la camera
    if (b_draw_grid) ::draw(grid, camera);
    if (b_draw_axe) ::draw(axe, camera);


     // LIGHT
    ::draw(cube, Translation( Vector( draw_param.light()))*Scale(0.3, 0.3, 0.3), camera);


    Transform Tc= Translation( -3, 5, 0 );
    ::draw(cube, Tc, camera, cube_texture);

    return 1;
}



int main( int argc, char **argv )
{
    Viewer v;
    v.run();
    return 0;
}
