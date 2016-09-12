
//! \file tuto9.cpp mesure du temps d'execution par le cpu et le gpu (utilise une requete / query openGL)

#include <cstdio>

#include "mat.h"
#include "mesh.h"
#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "draw.h"

#include "window.h"
#include "app.h"        // classe Application a deriver
#include "text.h"


class TP : public App
{
public:
    TP( ) : App(1024, 640) {}
    
    int init( )
    {
        m_objet= read_mesh("data/bigguy.obj");
        
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);

        m_texture= read_texture(0, "data/debug2x2red.png");

        // affichage du temps de glDraw
        m_console= create_text();

        // mesure du temps gpu de glDraw
        glGenQueries(1, &m_time_query);

        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre

        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest

        return 0;   // ras, pas d'erreur
    }
    
    int quit( )
    {
        glDeleteQueries(1, &m_time_query);
        release_text(m_console);
        
        m_objet.release();
        glDeleteTextures(1, &m_texture);
        
        return 0;
    }
    
    int update( const float time, const float delta )
    {
        m_model= RotationY(time / 20);
        return 0;
    }
    
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
        
        // mesure le temps d'execution du draw
        GLint64 cpu_start;
        GLint64 cpu_stop;
        
        glBeginQuery(GL_TIME_ELAPSED, m_time_query);    // pour le gpu
        glGetInteger64v(GL_TIMESTAMP, &cpu_start);      // pour le cpu
        
            draw(m_objet, m_model, m_camera, m_texture);
        
        glGetInteger64v(GL_TIMESTAMP, &cpu_stop);
        glEndQuery(GL_TIME_ELAPSED);
        
        // recupere le resultat
        GLint64 cpu_time= cpu_stop - cpu_start;
        GLint64 gpu_time= 0;
        glGetQueryObjecti64v(m_time_query, GL_QUERY_RESULT, &gpu_time);
        
        // affiche le temps mesure
        clear(m_console);
        printf(m_console, 0, 0, "cpu %dms %dus", (int) (cpu_time / 1000000), (int) ((cpu_time / 1000) % 1000));
        printf(m_console, 0, 1, "gpu %dms %dus", (int) (gpu_time / 1000000), (int) ((gpu_time / 1000) % 1000));

        // affiche le texte
        draw(m_console, window_width(), window_height());
        
        return 0;
    }

protected:
    GLuint m_time_query;
    Text m_console;
    Transform m_model;
    Mesh m_objet;
    GLuint m_texture;
    Orbiter m_camera;
};

int main( )
{
    TP tp;
    tp.run();
    
    return 0;
}
