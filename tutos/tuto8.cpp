
//! \file tuto8.cpp exemple d'animation en derivant App::update()

#include "mat.h"
#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "draw.h"
#include "app.h"        // classe Application a deriver


class TP : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : App(1024, 640) {}
    
    int init( )
    {
        m_objet= read_mesh("data/cube.obj");
        
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);

        m_texture= read_texture(0, "data/debug2x2red.png");

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
        if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.move(mx);
        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        
        draw(m_objet, m_model, m_camera, m_texture);
        
    /* on pourrait obtenir le meme resultat sans utiliser App::update( ). 
        les parametres time et update sont renvoyes par les fonctions  global_time() et delta_time() :
        draw(m_objet, RotationY(global_time() / 20), m_camera, m_texture);
    */
        
        return 1;
    }

protected:
    Transform m_model;
    Mesh m_objet;
    GLuint m_texture;
    Orbiter m_camera;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    
    return 0;
}
