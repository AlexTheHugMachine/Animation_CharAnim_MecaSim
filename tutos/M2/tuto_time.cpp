
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
        // mode 0 & 1
        m_objet= read_mesh("data/bigguy.obj");        
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        m_camera.lookat(pmin - Vector(20, 20, 0), pmax + Vector(20, 20, 0));

        m_texture= read_texture(0, "data/debug2x2red.png");
        
        // mode 2
        // configure le vertex array / format de sommet
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        // buffer : positions + normals 
        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, m_objet.vertex_buffer_size() + m_objet.normal_buffer_size(), 0, GL_STATIC_DRAW);
        
        // transfere les positions des sommets
        glBufferSubData(GL_ARRAY_BUFFER, /* offset */ 0, /* size */ m_objet.vertex_buffer_size(), /* data */ m_objet.vertex_buffer());
        // configure l'attribut 0, vec3 position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, /* offset */ 0);
        glEnableVertexAttribArray(0);
        
        // transfere les normales des sommets
        glBufferSubData(GL_ARRAY_BUFFER, /* offset */ m_objet.vertex_buffer_size(), /* size */ m_objet.normal_buffer_size(), /* data */ m_objet.normal_buffer());
        // configure l'attribut 2, vec3 normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, /* offset */ (const GLvoid *) m_objet.vertex_buffer_size());
        glEnableVertexAttribArray(2);
        
        m_vertex_count= m_objet.vertex_count();

        m_program= read_program("tutos/M2/instance.glsl");
        program_print_errors(m_program);
        
        // nettoyage 
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
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
        
        static int mode= 0;
        if(key_state(' '))
        {
            clear_key_state(' ');
            mode= (mode + 1) % 3;
        }
        
        if(mode == 0)
        {
            // dessine 1 objet
            draw(m_objet, m_model, m_camera, m_texture);
        }
        else if(mode == 1)
        {
            // dessine 25 fois l'objet sur une grille
            for(int y= -2; y <= 2; y++)
            for(int x= -2; x <= 2; x++)
            {
                Transform t= Translation(x *20, y *20, 0);
                draw(m_objet, t * m_model, m_camera, m_texture);
            }
        }
        else if(mode == 2)
        {
            // dessine 25 copies de l'objet sur une grille avec un seul glDrawArrayInstanced( ), c'est le vertex shader qui calcule la translation
            // cf gl_InstanceID qui contient l'indice de la copie
            glBindVertexArray(m_vao);
            glUseProgram(m_program);
            
            Transform m= m_model;
            Transform v= m_camera.view();
            Transform p= m_camera.projection(window_width(), window_height(), 45);
            Transform mvp= p * v * m;
            Transform mv= v * m;
            
            program_uniform(m_program, "mvpMatrix", mvp);
            program_uniform(m_program, "normalMatrix", mv.normal());
            
            glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertex_count, 25);
        }
    
        std::chrono::high_resolution_clock::time_point cpu_stop= std::chrono::high_resolution_clock::now();
        // conversion des mesures en duree...
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
        if(mode == 0) printf(m_console, 0, 0, "mode 0 : 1 draw");
        if(mode == 1) printf(m_console, 0, 0, "mode 1 : 25 draws");
        if(mode == 2) printf(m_console, 0, 0, "mode 2 : 1 draw / 25 instances");
        printf(m_console, 0, 1, "cpu  %02dms %03dus", (int) (cpu_time / 1000000), (int) ((cpu_time / 1000) % 1000));
        printf(m_console, 0, 2, "gpu  %02dms %03dus", (int) (gpu_time / 1000000), (int) ((gpu_time / 1000) % 1000));
        printf(m_console, 0, 3, "wait %02dms %03dus", (int) (wait_time / 1000000), (int) ((wait_time / 1000) % 1000));
        
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
    Orbiter m_camera;
    
    GLuint m_texture;
    GLuint m_program;
    GLuint m_vao;
    GLuint m_vertex_buffer;
    unsigned int m_vertex_count;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    
    return 0;
}
