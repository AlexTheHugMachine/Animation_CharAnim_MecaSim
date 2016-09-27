
//! \file tuto_time.cpp mesure du temps d'execution par le cpu et le gpu (utilise une requete / query openGL) + std::chrono pour le cpu

#include <chrono>

#include "mat.h"
#include "program.h"
#include "uniforms.h"
#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "draw.h"

#include "app.h"        // classe Application a deriver
#include "text.h"


class TP : public App
{
public:
    TP( ) : App(1024, 640) {}
    
    int init( )
    {
        // charge un objet et une texture a afficher
        m_objet= read_mesh("data/bigguy.obj");        
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);

        m_texture= read_texture(0, "data/debug2x2red.png");

        // mesure du temps gpu de glDraw
        glGenQueries(1, &m_time_query);

        // affichage du temps  dans la fenetre
        m_console= create_text();

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
        
        // mesure le temps d'execution du draw pour le gpu
        glBeginQuery(GL_TIME_ELAPSED, m_time_query);
        
        // mesure le temps d'execution du draw pour le cpu
        // utilise std::chrono pour mesurer le temps cpu 
        std::chrono::high_resolution_clock::time_point cpu_start= std::chrono::high_resolution_clock::now();
        
    #if 1
        // dessine 1 objet
        draw(m_objet, m_model, m_camera, m_texture);
        
    #else
        // dessine 25 fois l'objet sur une grille
        for(int y= -2; y <= 2; y++)
        for(int x= -2; x <= 2; x++)
        {
            Transform t= Translation(x *20, y *20, 0);
            draw(m_objet, t * m_model, m_camera, m_texture);
        }
    #endif
        
        std::chrono::high_resolution_clock::time_point cpu_stop= std::chrono::high_resolution_clock::now();
        // conversion desmesures en duree...
        long long int cpu_time= std::chrono::duration_cast<std::chrono::nanoseconds>(cpu_stop - cpu_start).count();
        
        glEndQuery(GL_TIME_ELAPSED);

        /* recuperer le resultat de la requete time_elapsed, il faut attendre que le gpu ait fini de dessiner...
            utilise encore std::chrono pour mesurer le temps d'attente.
         */
        std::chrono::high_resolution_clock::time_point wait_start= std::chrono::high_resolution_clock::now();
        
        // attendre le resultat de la requete
        GLint64 gpu_time= 0;
        glGetQueryObjecti64v(m_time_query, GL_QUERY_RESULT, &gpu_time);

        std::chrono::high_resolution_clock::time_point wait_stop= std::chrono::high_resolution_clock::now();
        long long int wait_time= std::chrono::duration_cast<std::chrono::nanoseconds>(wait_stop - wait_start).count();

        // affiche le temps mesure, et formate les valeurs... c'est un peu plus lisible.
        clear(m_console);
        printf(m_console, 0, 0, "cpu  %02dms %03dus", (int) (cpu_time / 1000000), (int) ((cpu_time / 1000) % 1000));
        printf(m_console, 0, 1, "gpu  %02dms %03dus", (int) (gpu_time / 1000000), (int) ((gpu_time / 1000) % 1000));
        printf(m_console, 0, 2, "wait %02dms %03dus", (int) (wait_time / 1000000), (int) ((wait_time / 1000) % 1000));
        
        // affiche le texte dans la fenetre de l'application, utilise console.h
        draw(m_console, window_width(), window_height());

        // affiche le temps dans le terminal 
        printf("cpu  %02dms %03dus    ", (int) (cpu_time / 1000000), (int) ((cpu_time / 1000) % 1000));
        printf("gpu  %02dms %03dus\n", (int) (gpu_time / 1000000), (int) ((gpu_time / 1000) % 1000));
        
        return 1;
    }
    
protected:
    GLuint m_time_query;
    Text m_console;
    
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
