
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "draw.h"        // pour dessiner du point de vue d'une camera
#include "Viewer.h"

using namespace std;


Viewer::Viewer() : App(1024, 768), b_draw_grid(true), b_draw_axe(true), b_draw_animation(false)
{
}


void Viewer::help()
{
    printf("HELP:\n");
    printf("\th: help\n");
    printf("\ta: (des)active l'affichage de l'axe\n");
    printf("\tg: (des)active l'affichage de la grille\n");
    printf("\tz: (des)active l'affichage de la courbe d'animation\n");
    printf("\tCtrl+fleche/pageUp/pageDown: bouge la source de lumière\n");
    printf("\tSouris+bouton gauche: rotation\n");
    printf("\tSouris mouvement horizontal+bouton droit: (de)zoom\n");
}

int Viewer::init()
{
    // etat par defaut openGL
    glClearColor(0.5f, 0.5f, 0.9f, 1);
    glClearDepthf(1);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    m_anim.init( "data/animation/anim1.ani");

    camera.lookat( Point(0,0,0), 30 );
    gl.light( Point(0, 20, 20), White() );

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


void Viewer::manageCameraLight()
{
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
    if (key_state(SDLK_DOWN) && (!key_state(SDLK_LCTRL))) { camera.move( -1); }
    if (key_state(SDLK_UP) && (!key_state(SDLK_LCTRL))) { camera.move( 1); }


    // Deplace la lumiere
    const float step = 0.1f;
    if (key_state(SDLK_RIGHT) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(step,0,0)); }
    if (key_state(SDLK_LEFT) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(-step,0,0)); }
    if (key_state(SDLK_UP) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(0,step,0)); }
    if (key_state(SDLK_DOWN) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(0,-step,0)); }
    if (key_state(SDLK_PAGEUP) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(0,0,step)); }
    if (key_state(SDLK_PAGEDOWN) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(0,0, -step)); }



    // (De)Active la grille / les axes
    if (key_state('h')) help();
    if (key_state('g')) { b_draw_grid = !b_draw_grid; clear_key_state('g'); }
    if (key_state('a')) { b_draw_axe = !b_draw_axe; clear_key_state('a'); }
    if (key_state('z')) { b_draw_animation=!b_draw_animation; clear_key_state('z');}

    gl.camera(camera);
    //draw(cube, Translation( Vector( gl.light()))*Scale(0.3, 0.3, 0.3), camera);
    //draw_param.texture(quad_texture).camera(camera).model(Translation( 3, 5, 0 )).draw(quad);

    // AXE et GRILLE
    gl.model( Identity() );
    if (b_draw_grid) gl.draw(grid);
    if (b_draw_axe) gl.draw(axe);
    if (b_draw_animation) m_anim.draw(camera);

     // LIGHT
    gl.texture( 0 );
    gl.model( Translation( Vector( gl.light()))*Scale(0.3, 0.3, 0.3) );
    gl.draw(cube);


}


int Viewer::render( )
{
    // Afface l'ecran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    manageCameraLight();

    gl.camera(camera);

    gl.texture(quad_texture);
    gl.model(Translation( 3, 5, 0 ));
    gl.draw(quad);

    gl.texture(cube_texture);
    gl.model(Translation( -3, 5, 0 ));
    gl.draw(cube);

    return 1;
}



