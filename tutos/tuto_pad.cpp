

//! \file tuto_pad.cpp exemple d'utilisation des pads.

#include "mat.h"
#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "draw.h"
#include "app.h"        // classe Application a deriver
#include "gamepads.h"


// utilitaire. creation d'une grille / repere.
Mesh make_grid( )
{
    Mesh grid= Mesh(GL_LINES);
    
    for(int x= 0; x < 10; x++)
    {
        float px= (float) x - 5.f + 0.5f;
        grid.vertex(Point(px, 0, -4.5f)); 
        grid.vertex(Point(px, 0, 4.5f)); 
    }

    for(int z= 0; z < 10; z++)
    {
        float pz= (float) z - 5.f + 0.5f;
        grid.vertex(Point(-4.5f, 0, pz)); 
        grid.vertex(Point(4.5f, 0, pz)); 
    }
    
    return grid;
}

class Pad : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    Pad( ) : App(1024, 640) {}
    
    int init( )
    {
        m_objet= read_mesh("data/cube.obj");
        m_grid= make_grid();
        
        Point pmin, pmax;
        m_grid.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);

        m_texture= read_texture(0, "data/debug2x2red.png");

        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest
        
        //
        if(!m_gamepads.create())
            return -1;
        
        return 0;   // ras, pas d'erreur
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        m_objet.release();
        m_grid.release();
        glDeleteTextures(1, &m_texture);
        
        //
        m_gamepads.release();
        return 0;
    }
    
    int update( const float time, const float delta )
    {
        m_gamepads.update();
        
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // deplace la camera
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.move(mx);
        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        
        static float position= 0;
        //~ float speed= m_gamepads.axis(0, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        float speed= m_gamepads.pad(0).axis(SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        position= position + speed;
        
        static float rotation= 0;
        //~ float angle= m_gamepads.axis(0, SDL_CONTROLLER_AXIS_RIGHTX);
        float angle= m_gamepads.pad(0).axis(SDL_CONTROLLER_AXIS_RIGHTX);
        rotation= rotation + angle;
        
        m_model= Translation(0, 0, -position) * RotationY(-rotation) * RotationX(-global_time() / 4);
        
        draw(m_grid, m_camera);
        draw(m_objet, m_model, m_camera, m_texture);
        
        // quitter
        int stop= 1;
        //~ if(m_gamepads.button(0, SDL_CONTROLLER_BUTTON_BACK))
        if(m_gamepads.pad(0).button(SDL_CONTROLLER_BUTTON_BACK))
            stop= 0;
        return stop;
    }

protected:
    Transform m_model;
    Mesh m_objet;
    Mesh m_grid;

    GLuint m_texture;
    Orbiter m_camera;
    Gamepads m_gamepads;
};


int main( int argc, char **argv )
{
    Pad app;
    app.run();
    
    return 0;
}
