
#include <cassert>
#include <math.h>
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
    init_sphere();
    init_cylinder();

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
    static float pt[8][3] = { {-1,-1,-1}, {1,-1,-1}, {1,-1,1}, {-1,-1,1}, {-1,1,-1}, {1,1,-1}, {1,1,1}, {-1,1,1} };
    static int f[6][4] = { {0,1,2,3}, {5,4,7,6}, {1,5,6,2}, {0,3,7,4}, {3,2,6,7}, {0,4,5,1} };
    static float n[6][3] = { {0,-1,0}, {0,1,0}, {1,0,0}, {-1,0,0}, {0,0,1}, {0,0,-1} };
    static float uv[4][2] = { {0,0}, {1,0}, {1,1}, {0,1} };
    int i,j;

    cube = create_mesh(GL_TRIANGLES);
    vertex_color(cube, make_color(1, 1, 1));

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


void Viewer::init_sphere()
{
    const int divBeta = 26;
    const int divAlpha = divBeta/2;
    int i,j;
    float beta, alpha, alpha2;

    sphere = create_mesh(GL_TRIANGLE_STRIP);

    for(i=0;i<divAlpha;++i)
    {
        alpha = -0.5f*M_PI + float(i)*M_PI/divAlpha;
        alpha2 = -0.5f*M_PI + float(i+1)*M_PI/divAlpha;

        for(j=0;j<divBeta;++j)
        {
            beta = float(j)*2.f*M_PI/(divBeta-1);

            //glTexCoord2f( beta/(2.0f*M_PI), 0.5f+alpha/M_PI );
            //glNormal3f( cos(alpha)*cos(beta),  sin(alpha), cos(alpha)*sin(beta) );
            //glVertex3f( cos(alpha)*cos(beta),  sin(alpha), cos(alpha)*sin(beta) );
            push_vertex( sphere,
                        Point(cos(alpha)*cos(beta),  sin(alpha), cos(alpha)*sin(beta)),
                        Vector(cos(alpha)*cos(beta),  sin(alpha), cos(alpha)*sin(beta))
                         );


            //glTexCoord2f( beta/(2.0f*M_PI), 0.5f+alpha2/M_PI );
            //glNormal3f( cos(alpha2)*cos(beta),  sin(alpha2), cos(alpha2)*sin(beta) );
            //glVertex3f( cos(alpha2)*cos(beta),  sin(alpha2), cos(alpha2)*sin(beta) );
            push_vertex( sphere,
                        Point(cos(alpha2)*cos(beta),  sin(alpha2), cos(alpha2)*sin(beta)),
                        Vector(cos(alpha2)*cos(beta),  sin(alpha2), cos(alpha2)*sin(beta))
                        );
        }
        restart_strip(sphere);
    }
}


void Viewer::init_cylinder()
{
    int i;
    const int div = 25;
    float alpha;
    float step = 2.0*M_PI / (div);

    cylinder = create_mesh(GL_TRIANGLE_STRIP);

    for (i=0;i<=div;++i)
    {
        alpha = i*step;
        //glNormal3f( cos(alpha),  0, sin(alpha) );
        //glTexCoord2f( float(i)/div, 0.f );
        //glVertex3f( cos(alpha),  0, sin(alpha) );
        //glTexCoord2f( float(i)/div, 1.f );
        //glVertex3f( cos(alpha),  1, sin(alpha) );
        push_vertex( cylinder,
                            Point(cos(alpha),  -1, sin(alpha)),
                            Vector(cos(alpha),  0, sin(alpha)) );

        push_vertex( cylinder,
                            Point(cos(alpha),  1, sin(alpha)),
                            Vector(cos(alpha),  0, sin(alpha)) );
    }
}


void Viewer::init_terrain()
{
    terrain = create_mesh(GL_TRIANGLE_STRIP);
    Image im( "data/terrain.png");


}


int Viewer::quit( )
{
    return 0;
}


void Viewer::draw_plane(const Transform& T)
{
    //glMatrixMode(GL_MODELVIEW);

    Transform P = T;
    P *= make_translation(0,3,0);
    P *= make_scale(0.1f,0.1f, 0.1f);


    Transform F = P * make_scale(1,1,10);
    ::draw( sphere, F, camera);

    Transform W = P * make_translation(0,0,2) * make_scale(10,0.1,1);
    ::draw( cube, W, camera);

    Transform WR = P * make_translation(0,0,-8) * make_scale( 3, 0.1, 0.5);
    ::draw( cube, WR, camera);

    Transform WRV = P * make_translation(0,1.5,-8) * make_scale( 0.1, 1.5, 0.5);
    ::draw( cube, WRV, camera);

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

    Transform T= make_translation( -3, 1, 0 );
    ::draw(cube, T, camera);

    T= make_translation( 0, 1, 0 );
    ::draw( cylinder, T, camera);

    T= make_translation( 3, 1, 0 );
    ::draw(sphere, T, camera);

    draw_plane( Transform()  );

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
