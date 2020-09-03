
//! \file tuto9.cpp utilisation d'un shader 'utilisateur' pour afficher un objet Mesh

#include <cmath>

#include "mat.h"
#include "mesh.h"
#include "wavefront.h"

#include "orbiter.h"
#include "program.h"
#include "uniforms.h"
#include "draw.h"

#include "app_time.h"        // classe Application a deriver



// utilitaire. creation d'une grille / repere.
Mesh make_grid( const int n= 10 )
{
    Mesh grid= Mesh(GL_LINES);
    
    // grille
    grid.color(White());
    for(int x= 0; x < n; x++)
    {
        float px= float(x) - float(n)/2 + .5f;
        grid.vertex(Point(px, 0, - float(n)/2 + .5f)); 
        grid.vertex(Point(px, 0, float(n)/2 - .5f));
    }

    for(int z= 0; z < n; z++)
    {
        float pz= float(z) - float(n)/2 + .5f;
        grid.vertex(Point(- float(n)/2 + .5f, 0, pz)); 
        grid.vertex(Point(float(n)/2 - .5f, 0, pz)); 
    }
    
    // axes XYZ
    grid.color(Red());
    grid.vertex(Point(0, 0.1, 0));
    grid.vertex(Point(1, 0.1, 0));
    
    grid.color(Green());
    grid.vertex(Point(0, 0.1, 0));
    grid.vertex(Point(0, 1.1, 0));
    
    grid.color(Blue());
    grid.vertex(Point(0, 0.1, 0));
    grid.vertex(Point(0, 0, 1));
    
    return grid;
}

// utilise AppTime pour les screenshots...
class TP : public AppTime
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : AppTime(1024, 640) {}
    
    int init( )
    {
        m_grid= make_grid();
        m_objet= read_mesh("data/cube.obj");
        
        Point pmin, pmax;
        m_grid.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);

        // etape 1 : creer le shader program
        m_program= read_program("tutos/tuto10.glsl");
        program_print_errors(m_program);
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest

        return 0;   // ras, pas d'erreur
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        // etape 3 : detruire le shader program
        release_program(m_program);
        // et les objets
        m_grid.release();
        m_objet.release();
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // recupere l'etat de la souris
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        
        // deplace la camera
        if(mb & SDL_BUTTON(1))
            m_camera.rotation(mx, my);      // tourne autour de l'objet
        else if(mb & SDL_BUTTON(3))
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height()); // deplace le point de rotation
        else if(mb & SDL_BUTTON(2))
            m_camera.move(mx);           // approche / eloigne l'objet
        
        // recupere l'etat de la molette / touch
        SDL_MouseWheelEvent wheel= wheel_event();
        if(wheel.y != 0)
        {
            clear_wheel_event();
            m_camera.move(8.f * wheel.y);  // approche / eloigne l'objet
        }
        
        // recharge le shader program
        if(key_state('r'))
        {
            clear_key_state('r');        // une seule fois...
            reload_program(m_program, "tutos/tuto10.glsl");
            program_print_errors(m_program);
        }
        
        // etape 2 : dessiner m_objet avec le shader program
        // configurer le pipeline 
        glUseProgram(m_program);

        // configurer le shader program
        // . recuperer les transformations
        Transform model= RotationX(global_time() / 20);
        Transform view= m_camera.view();
        Transform projection= m_camera.projection(window_width(), window_height(), 45);
        
        // . composer les transformations : model, view et projection
        Transform mv= view * model;
        Transform mvp= projection * mv;
        
        // . parametrer le shader program :
        //   . transformation : la matrice declaree dans le vertex shader s'appelle mvpMatrix
        program_uniform(m_program, "mvpMatrix", mvp);
        
        // . parametres "supplementaires" :
        
        // go !
        // mesh associe les donnees positions, texcoords, normals et colors aux attributs declares dans le vertex shader
        m_objet.draw(m_program, /* use position */ true, /* use texcoord */ false, /* use normal */ false, /* use color */ false, /* use material index*/ false);
        // relisez le tuto9 sur l'utilisation des shaders et de mesh

        // et pour afficher un autre cube d'une autre couleur ?

        // dessine aussi le repere et la grille pour le meme point de vue
        draw(m_grid, Identity(), m_camera);
        
        return 1;
    }

protected:
    Mesh m_objet;
    Mesh m_grid;
    Orbiter m_camera;
    GLuint m_program;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    
    return 0;
}

