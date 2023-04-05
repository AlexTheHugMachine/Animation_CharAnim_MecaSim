/** \file Viewer-init.cpp
 * \brief Fonctions d initialisation des Mesh des objets a afficher.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "draw.h"
#include "Viewer.h"

using namespace std;





/*
 * Creation du maillage des axes pour les visualiser facilement.
 */
void Viewer::init_axe()
{
    m_axe = Mesh(GL_LINES);
    
    // Axe x en rouge
    m_axe.color( Color(1, 0, 0));
    m_axe.vertex( 0,  0, 0);
    m_axe.vertex( 1,  0, 0);
    
    // Axe y en vert
    m_axe.color( Color(0, 1, 0));
    m_axe.vertex( 0,  0, 0);
    m_axe.vertex( 0,  1, 0);
    
    // Axe z en bleu
    m_axe.color( Color( 0, 0, 1));
    m_axe.vertex( 0,  0, 0);
    m_axe.vertex( 0,  0, 1);
    
}


/*
 * Creation du maillage d'une grille.
 */
void Viewer::init_grid()
{
    m_grid = Mesh(GL_LINES);
    
    // Couleur de la grille
    m_grid.color( Color(1, 1, 1));
    
    for(int i=-5;i<=5;++i)
        for(int j=-5;j<=5;++j)
        {
            m_grid.vertex( -5, 0, j);
            m_grid.vertex( 5, 0,  j);
            
            m_grid.vertex( i, 0, -5);
            m_grid.vertex( i, 0, 5);
            
        }
}


/*
 * Creation du maillage du plan de collision.
 */
void Viewer::init_plan(float x, float y, float z)
{
    m_plan = Mesh(GL_LINES);
    
    // Couleur de la grille
    m_plan.color( Color(1, 0, 0));
    
    for(int i=-5;i<=5;++i)
        for(int j=-5;j<=5;++j)
        {
            m_plan.vertex( -5+x, y, j+z);
            m_plan.vertex( 5+x, y,  j+z);
            
            m_plan.vertex( i+x, y, -5+z);
            m_plan.vertex( i+x, y, 5+z);
            
        }
}


/*
 * Creation du maillage d un cube.
 */
void Viewer::init_cube()
{
    // Definition des 8 sommets du cube
    //                          0           1           2       3           4           5       6           7
    // 8 sommets - chaque sommet a coodonnees en x, y, z
    static float pt[8][3] = { {-1,-1,-1}, {1,-1,-1}, {1,-1,1}, {-1,-1,1}, {-1,1,-1}, {1,1,-1}, {1,1,1}, {-1,1,1} };
   
    // 6 faces - chacune des faces a 4 sommets
    static int f[6][4] = {    {0,1,2,3}, {5,4,7,6}, {2,1,5,6}, {0,3,7,4}, {3,2,6,7}, {1,0,4,5} };
    
    // 6 normales (une normale par face) - chaque normale a coodonnees en x, y, z
    static float n[6][3] = { {0,-1,0}, {0,1,0}, {1,0,0}, {-1,0,0}, {0,0,1}, {0,0,-1} };

    
    // Maillage de type GL_TRIANGLE_STRIP
    m_cube = Mesh(GL_TRIANGLE_STRIP);
    
    // Couleur du cube
    m_cube.color( Color(1, 1, 1) );
    
    // Boucle sur les 6 faces du cube
    for (int i=0;i<6;i++)
    {
        // Definition de la normale a la face i
        m_cube.normal(  n[i][0], n[i][1], n[i][2] );
        
        // Definition du sommet 0 de la face i : coordonnees texture + coordonnees geometrique
        m_cube.texcoord( 0,0 );
        m_cube.vertex( pt[ f[i][0] ][0], pt[ f[i][0] ][1], pt[ f[i][0] ][2] );
        
        // Definition du sommet 1 de la face i  : coordonnees texture + coordonnees geometrique
        m_cube.texcoord( 1,0);
        m_cube.vertex( pt[ f[i][1] ][0], pt[ f[i][1] ][1], pt[ f[i][1] ][2] );
        
        // Definition du sommet 3 de la face i  : coordonnees texture + coordonnees geometrique
        m_cube.texcoord(0,1);
        m_cube.vertex(pt[ f[i][3] ][0], pt[ f[i][3] ][1], pt[ f[i][3] ][2] );
        
        // Definition du sommet 2 de la face i : coordonnees texture + coordonnees geometrique
        m_cube.texcoord(1,1);
        m_cube.vertex( pt[ f[i][2] ][0], pt[ f[i][2] ][1], pt[ f[i][2] ][2] );
        
        m_cube.restart_strip();
        
    }
}

void Viewer::init_cube_inv()
{
    // Definition des 8 sommets du cube
    //                          0           1           2       3           4           5       6           7
    // 8 sommets - chaque sommet a coodonnees en x, y, z
    static float pt[8][3] = { {-1,-1,-1}, {1,-1,-1}, {1,-1,1}, {-1,-1,1}, {-1,1,-1}, {1,1,-1}, {1,1,1}, {-1,1,1} };
   
    // 6 faces - chacune des faces a 4 sommets
    static int f[6][4] = {    {0,1,2,3}, {5,4,7,6}, {2,1,5,6}, {0,3,7,4}, {3,2,6,7}, {1,0,4,5} };
    
    // 6 normales (une normale par face) - chaque normale a coodonnees en x, y, z
    static float n[6][3] = { {0,-1,0}, {0,1,0}, {1,0,0}, {-1,0,0}, {0,0,1}, {0,0,-1} };

    
    // Maillage de type GL_TRIANGLE_STRIP
    m_cube_inv = Mesh(GL_TRIANGLE_STRIP);
    
    // Couleur du cube
    m_cube_inv.color( Color(1, 1, 1) );
    
    // Boucle sur les 6 faces du cube
    for (int i=0;i<6;i++)
    {
        // Definition de la normale a la face i
        m_cube_inv.normal(  n[i][2], n[i][1], n[i][0] );
        
        // Definition du sommet 0 de la face i : coordonnees texture + coordonnees geometrique
        m_cube_inv.texcoord( 0,0 );
        m_cube_inv.vertex( pt[ f[i][0] ][2], pt[ f[i][0] ][1], pt[ f[i][0] ][0] );
        
        // Definition du sommet 1 de la face i  : coordonnees texture + coordonnees geometrique
        m_cube_inv.texcoord( 1,0);
        m_cube_inv.vertex( pt[ f[i][1] ][2], pt[ f[i][1] ][1], pt[ f[i][1] ][0] );
        
        // Definition du sommet 3 de la face i  : coordonnees texture + coordonnees geometrique
        m_cube_inv.texcoord(0,1);
        m_cube_inv.vertex(pt[ f[i][3] ][2], pt[ f[i][3] ][1], pt[ f[i][3] ][0] );
        
        // Definition du sommet 2 de la face i : coordonnees texture + coordonnees geometrique
        m_cube_inv.texcoord(1,1);
        m_cube_inv.vertex( pt[ f[i][2] ][2], pt[ f[i][2] ][1], pt[ f[i][2] ][0] );
        
        m_cube_inv.restart_strip();
        
    }
}


/*
 * Creation du maillage d'une sphere - centre (0, 0, 0) - rayon = r.
 */
void Viewer::init_sphere()
{
    const int divBeta = 26;
    const int divAlpha = divBeta/2;
    int i,j;
    float beta, alpha, alpha2;
    float rayon = 0.05;
    
    m_sphere = Mesh(GL_TRIANGLE_STRIP);
    
    m_sphere.color( Color(0, 0, 1) );
        
    for(i=0;i<divAlpha;++i)
    {
        alpha = -0.5f*M_PI + float(i)*M_PI/divAlpha;
        alpha2 = -0.5f*M_PI + float(i+1)*M_PI/divAlpha;
        
        for(j=0;j<divBeta;++j)
        {
            beta = float(j)*2.f*M_PI/(divBeta-1);
            
            m_sphere.texcoord(beta/(2.0f*M_PI), 0.5f+alpha2/M_PI);
            
            m_sphere.normal( - Vector(rayon * cos(alpha2)*cos(beta),
                                    rayon * sin(alpha2),
                                    rayon * cos(alpha2)*sin(beta)) );
            
            m_sphere.vertex( Point(rayon * cos(alpha2)*cos(beta),
                                   rayon*sin(alpha2),
                                   rayon * cos(alpha2)*sin(beta))   );
            
            
            m_sphere.texcoord(beta/(2.0f*M_PI), 0.5f+alpha/M_PI);
            
            m_sphere.normal( - Vector(rayon * cos(alpha)*cos(beta),
                                    rayon * sin(alpha),
                                    rayon * cos(alpha)*sin(beta)) );
            
            m_sphere.vertex( Point(rayon * cos(alpha)*cos(beta),
                                   rayon * sin(alpha),
                                   rayon * cos(alpha)*sin(beta)) );
        }
        
        m_sphere.restart_strip();
    }
}

void Viewer::init_disque()
{
    m_disque = Mesh(GL_TRIANGLE_FAN);
    //m_disque.color(1, 0, 0);

    int nb_sommets = 20.0;
    float istep = (2.0*M_PI)/nb_sommets;
    float theta=0;

    m_disque.normal(0, 1, 0);
    m_disque.vertex(0,0,0);

    for (int i=0; i<=nb_sommets+1; i++)
    {
        m_disque.texcoord(float(i+1)/nb_sommets, 1.0 - float(i+1)/nb_sommets);
        m_disque.vertex(Point(sin(theta), 0, cos(theta)));
        theta=i*istep;
    }
}

void Viewer::init_cylindre()
{
    int nb_sommets = 20;
    float step = (2.0*M_PI)/nb_sommets;

    m_cylindre = Mesh(GL_TRIANGLE_STRIP);

    for (int i = 0; i<=nb_sommets; i++)
    {
        float alpha = i*step;
        m_cylindre.normal(Vector(sin(alpha), 0, cos(alpha)));
        m_cylindre.texcoord(float(i)/nb_sommets, 0.0);
        m_cylindre.vertex(Point(sin(alpha), 0, cos(alpha)));
        m_cylindre.normal(Vector(sin(alpha), 0, cos(alpha)));
        m_cylindre.texcoord(float(i)/nb_sommets, 1.0);
        m_cylindre.vertex(Point(sin(alpha), 5, cos(alpha)));
    }
}

void Viewer::init_cone()
{
    int nb = 26;
    float obj = 2.0*M_PI/nb;
    float alpha = 0.0;
    m_cone = Mesh(GL_TRIANGLE_FAN);
    m_cone.texcoord(0.5, 0.5);
    m_cone.vertex(0, 1, 0);
    for (int i=0; i<=nb+1; i++)
    {
        m_cone.normal(sin(alpha), 0, cos(alpha));
        m_cone.texcoord(0.5 + (cos(alpha)*0.5), 0.5 + (sin(alpha)*0.5));
        m_cone.vertex(sin(alpha), 0.0, cos(alpha));
        alpha=i*obj;
    }
}

void Viewer::init_cubemap()
{

    static float pt[8][3] = {{-50,-50,-50}, {50,-50,-50}, {50,-50,50}, {-50,-50,50}, {-50,50,-50}, {50,50,-50}, {50,50,50}, {-50,50,50}};
    static float tex_pt[14][2] = {{0,0.33}, {0,0.66}, {0.25,0.33}, {0.25,0.66},{0.5,0.33}, {0.5,0.66}, {0.75,0.33}, {0.75,0.66}, {0.98,0.33}, {0.98,0.66}, {0.25,0.99}, {0.5,0.99}, {0.25,0}, {0.5,0}};
    static int tex[6][4] = {{2,4,12,13}, {10,11,3,5}, {6,7,8,9}, {2,3,4,5}, {4,5,6,7},{0,1,2,3}};
    static int f[6][4] = {{0,1,2,3}, {5,4,7,6}, {2,1,5,6}, {0,3,7,4}, {3,2,6,7}, {1,0,4,5}};
    static float n[6][3] = {{0,1,0}, {0,-1,0}, {-1,0,0}, {1,0,0}, {0,0,-1}, {0,0,1}};
    int i,j;

    m_cubemap = Mesh(GL_TRIANGLE_STRIP);
    //m_tex_cubemap = read_texture(0, smart_path("data/cubemap/skybox.png")) ;
    m_cubemap.color(1,1,1);

    for (i=0;i<6;i++){
        m_cubemap.normal(  n[i][2], n[i][1], n[i][0] );

        m_cubemap.texcoord( tex_pt[tex[i][0]][1],tex_pt[tex[i][0]][0] );
        m_cubemap.vertex( pt[ f[i][0] ][2], pt[ f[i][0] ][1], pt[ f[i][0] ][0] );

        m_cubemap.texcoord( tex_pt[tex[i][1]][1],tex_pt[tex[i][1]][0] );
        m_cubemap.vertex(pt[ f[i][3] ][2], pt[ f[i][3] ][1], pt[ f[i][3] ][0] );

        m_cubemap.texcoord( tex_pt[tex[i][2]][1],tex_pt[tex[i][2]][0] );
        m_cubemap.vertex( pt[ f[i][1] ][2], pt[ f[i][1] ][1], pt[ f[i][1] ][0] );

        m_cubemap.texcoord( tex_pt[tex[i][3]][1],tex_pt[tex[i][3]][0] );
        m_cubemap.vertex( pt[ f[i][2] ][2], pt[ f[i][2] ][1], pt[ f[i][2] ][0] );
        m_cubemap.restart_strip();
    }
}

void Viewer::init_triangle()
{
    m_triangle = Mesh(GL_TRIANGLES);
    Point a(0, 0, 0);
    Point b(1, 0, 0);
    Point c(0, 0, 1);


    Vector AB = normalize(b-a);
    Vector AC = normalize(c-a);
    Vector N = cross(AB, AC);
    m_triangle.normal(N.x, N.y, N.z);

    m_triangle.texcoord(0, 0);
    m_triangle.vertex(a.x, a.y, a.z);

    m_triangle.texcoord(1, 0);
    m_triangle.vertex(b.x, b.y, b.z);

    m_triangle.texcoord(0, 1);
    m_triangle.vertex(c.x, c.y, c.z);
}

/*
 * Fonction d initialisation.
 */
int Viewer::init()
{
    // Etats par defaut openGL
    glClearColor(0.5f, 0.6f, 0.8f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepthf(1);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    
    if (mb_cullface)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);        // good for debug
    glEnable(GL_TEXTURE_2D);
    
    // Camera
    m_camera.lookat( Point(0,0,0), 15 );
    // m_camera.lookat( Point(0,0,0), 150 );
    
    // Lumiere
    gl.light( Point(5, 5, 5), White() );
    //gl.light2( Point(-20, -20, -20), White() );
    
    // Chargement des textures utilisees dans la scene
    // Texture pour le cube
   //  m_cube_texture = read_texture(0, "../data/debug2x2red.png");
    m_cube_texture = read_texture(0, smart_path("data/sky.jpg"));
    m_box_texture = read_texture(0, smart_path("data/box1.jpg"));
    m_cubemap_texture = read_texture(0, smart_path("data/sky.jpg"));
    m_cone_texture = read_texture(0, smart_path("data/Skybox.jpg"));
    m_cylindre_texture = read_texture(0, smart_path("data/pringles.png"));

    // Texture pour le tissu
    //m_tissu_texture = read_texture(0, "data/papillon.png");
    m_tissu_texture = read_texture(0, "data/textures/leffe.png");
   // m_tissu_texture = read_texture(0, "data/textures/tissu2.jpg");
    //m_tissu_texture = read_texture(0, smart_path("data/textures/tissu2.jpg"));

    // Appel des procedures d initialisation des objets de la scene
    // Pour les objets non simules
    // Exemple : init_votreObjet();
    init_axe();
    init_grid();
    init_cube();
    init_cube_inv();
    init_sphere();
    init_cone();
    init_cubemap();
    init_cylindre();
    init_disque();
    init_triangle();
    
    // Creation du plan (x, y, z) - plan utilise pour les ObjetSimule::Collision(x, y, z);
    // Rq : pas vraiment le plan, mais < x, < y, < z
    //init_plan(0, 0, 0);
    
    // Initialisation du Tps
    Tps = 0;
    
    // Point interaction
    MousePos = Vector(0, 0, 0);
    
    return 0;
    
}


