
//! \file tuto_transform.cpp visualisation du volume visible par une camera, + visualisation d'un objet (par rapport au volume visible)

#include "mat.h"
#include "wavefront.h"

#include "orbiter.h"
#include "draw.h"
#include "app.h"


class TP : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : App(1024, 640) {}
    
    int init( )
    {
        // volume visible par une camera, un cube -1 1
        m_frustum= read_mesh("data/frustum.obj");
        
        // grille / plan de reference
        m_grid.create(GL_LINES);
        for(int x= 0; x < 10; x++)
        {
            float px= (float) x - 5.f + 0.5f;
            m_grid.vertex(px, 0, -4.5f); 
            m_grid.vertex(px, 0, 4.5f); 
        }

        for(int z= 0; z < 10; z++)
        {
            float pz= (float) z - 5.f + 0.5f;
            m_grid.vertex(-4.5f, 0, pz); 
            m_grid.vertex(4.5f, 0, pz); 
        }
        
        // charge uin objet a afficher
        m_objet= read_mesh("data/bigguy.obj");

        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);
        //~ m_camera.lookat(Point(), 10);
        
        m_objet.default_color(Green());
        
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
        m_frustum.release();
        m_grid.release();
        m_objet.release();
        glDeleteTextures(1, &m_texture);
        
        return 0;
    }
    
    int update( const float time, const float delta )
    {
        // modifier l'orientation du cube a chaque image. 
        // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
        // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.
        
        m_model= RotationY(time / 20);
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // deplace la camera
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        
        if(key_state(' '))
        {
            // deplace la camera "normale"
            if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
                m_camera.rotation((float) mx, (float) my);
            else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
                m_camera.move((float) mx);
            else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
                m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        }
        else
        {
            // deplace l'observateur
            if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
                m_observer.rotation((float) mx / 10, (float) my / 10);
            else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
                m_observer.move((float) mx / 10);
            else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
                m_observer.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        }
    
        Transform view= m_observer.view();
        Transform projection= Perspective(45, (float) window_width() / (float) window_height(), .1f, 1000.f);

        static bool wireframe= false;
        if(key_state(' '))
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            // afficher le point de vue "normal"
            draw(m_grid, m_camera);
            draw(m_objet, m_model, m_camera);
        }
        else
        {
            if(key_state('w'))
            {
                clear_key_state('w');
                wireframe= !wireframe;
            }
            
            if(!wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
            // afficher le volume visible de la camera
            draw(m_grid, Identity(), view, projection);
            draw(m_frustum, Inverse(m_camera.projection(window_width(), window_height(), 45) * m_camera.view()), view, projection);
            draw(m_objet, m_model, view, projection);
        }
        
        return 1;
    }

protected:
    Transform m_model;
    Mesh m_frustum;
    Mesh m_objet;
    Mesh m_grid;
    GLuint m_texture;
    Orbiter m_camera;
    Orbiter m_observer;
};

int main( )
{
    TP tp;
    tp.run();
    
    return 0;
}
