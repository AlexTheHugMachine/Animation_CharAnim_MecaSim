
//! \file tuto_mdi.cpp affichage de plusieurs objets avec glMultiDrawIndirect() + mesure du temps d'execution par le cpu et le gpu (utilise une requete / query openGL)

#include <chrono>

#include "mat.h"
#include "program.h"
#include "uniforms.h"

#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "draw.h"

#include "app.h"
#include "text.h"


// representation des parametres 
struct IndirectParam
{
    unsigned int vertex_count;
    unsigned int instance_count;
    unsigned int first_vertex;
    unsigned int first_instance;
};



class TP : public App
{
public:
    TP( ) : App(1024, 640, 4, 3) {}     // openGL version 4.3, ne marchera pas sur mac.
    
    int init( )
    {
        //  verifie l'existence des extensions
        if(!GLEW_ARB_shader_draw_parameters)
            return -1;
        printf("GL_ARB_shader_draw_parameters ON\n");
            
        m_objet= read_mesh("data/bigguy.obj");
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        
        m_camera.lookat(pmin - Vector(200, 200,  0), pmax + Vector(200, 200, 0));
        
        // genere les parametres des draws et les transformations
        for(int y= -15; y <= 15; y++)
        for(int x= -15; x <= 15; x++)
        {
            m_multi_model.push_back( Translation(x *20, y *20, 0) );
            m_multi_indirect.push_back( { unsigned(m_objet.vertex_count()), 1, 0, 0} );
        }
        // oui c'est la meme chose qu'un draw instancie, mais c'est juste pour comparer les 2 solutions...
        
        // stockage des parametres du multi draw indirect
        glGenBuffers(1, &m_indirect_buffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirect_buffer);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(IndirectParam) * m_multi_indirect.size(), &m_multi_indirect.front(), GL_DYNAMIC_DRAW);   
        
        // stockage des matrices des objets
        glGenBuffers(1, &m_model_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_model_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * m_multi_model.size(), &m_multi_model.front(), GL_DYNAMIC_DRAW);   
        
        // creation des vertex buffer, uniquement les positions
        m_vao= m_objet.create_buffers(/* texcoord */ false, /* normal */ false, /* color */ false, /* material */ false);
        
        // shader programs
        m_program_direct= read_program("tutos/M2/indirect_direct.glsl");        // affichage classique N draws
        program_print_errors(m_program_direct);
        
        m_program= read_program("tutos/M2/indirect.glsl");      // affichage indirect 1 multi draw
        program_print_errors(m_program);
        
        // affichage du temps cpu / gpu
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
        
        release_program(m_program);
        m_objet.release();
        glDeleteBuffers(1, &m_indirect_buffer);
        
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
        glBeginQuery(GL_TIME_ELAPSED, m_time_query);    // pour le gpu
        std::chrono::high_resolution_clock::time_point cpu_start= std::chrono::high_resolution_clock::now();    // pour le cpu
        
    #if 0
        // dessine n copies de l'objet avec 1 glDrawArrays par copie
        glBindVertexArray(m_vao);
        glUseProgram(m_program_direct);
        
        program_uniform(m_program_direct, "modelMatrix", m_model);
        program_uniform(m_program_direct, "vpMatrix", m_camera.projection(window_width(), window_height(), 45) * m_camera.view());
        program_uniform(m_program_direct, "viewMatrix", m_camera.view());
        
        // dessine l'objet avec 1 draw par copie
        for(int i= 0; i < int(m_multi_model.size()); i++)
        {
            program_uniform(m_program_direct, "objectMatrix", m_multi_model[i]);
            glDrawArrays(GL_TRIANGLES, 0, m_objet.vertex_count());
        }
        // dans ce cas particulier, on pourrait utiliser un draw instancie, mais ce n'est pas le but du tuto...
        
    #else
        // dessine n copies de l'objet avec 1 seul appel a glMultiDrawArraysIndirect
        glBindVertexArray(m_vao);
        glUseProgram(m_program);
        
        // uniforms...
        program_uniform(m_program, "modelMatrix", m_model);
        program_uniform(m_program, "vpMatrix", m_camera.projection(window_width(), window_height(), 45) * m_camera.view());
        program_uniform(m_program, "viewMatrix", m_camera.view());
        
        // buffers...
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,  m_model_buffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirect_buffer);
        
        glMultiDrawArraysIndirect(m_objet.primitives(), 0, m_multi_indirect.size(), 0);
    #endif
        
        
        // affiche le temps 
        std::chrono::high_resolution_clock::time_point cpu_stop= std::chrono::high_resolution_clock::now();
        long long int cpu_time= std::chrono::duration_cast<std::chrono::nanoseconds>(cpu_stop - cpu_start).count();
        
        glEndQuery(GL_TIME_ELAPSED);
        
        // recupere le resultat de la requete gpu
        GLint64 gpu_time= 0;
        glGetQueryObjecti64v(m_time_query, GL_QUERY_RESULT, &gpu_time);
        
        clear(m_console);
        printf(m_console, 0, 0, "cpu  %02dms %03dus", (int) (cpu_time / 1000000), (int) ((cpu_time / 1000) % 1000));
        printf(m_console, 0, 1, "gpu  %02dms %03dus", (int) (gpu_time / 1000000), (int) ((gpu_time / 1000) % 1000));
        
        draw(m_console, window_width(), window_height());
        
        printf("cpu    %02dms %03dus    ", (int) (cpu_time / 1000000), (int) ((cpu_time / 1000) % 1000));
        printf("gpu    %02dms %03dus\n", (int) (gpu_time / 1000000), (int) ((gpu_time / 1000) % 1000));
        
        return 1;
    }
    
protected:
    GLuint m_indirect_buffer;
    GLuint m_model_buffer;
    GLuint m_time_query;
    
    GLuint m_vao;
    GLuint m_program;
    GLuint m_program_direct;

    Text m_console;

    Transform m_model;
    Mesh m_objet;
    Orbiter m_camera;
    
    std::vector<IndirectParam> m_multi_indirect;
    std::vector<Transform> m_multi_model;

};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    
    return 0;
}
