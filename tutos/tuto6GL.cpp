
//! \file tuto6GL.cpp utilisation de glDrawArraysInstanced() et gl_InstanceID

#include "vec.h"
#include "mat.h"

#include "mesh.h"
#include "wavefront.h"

#include "program.h"
#include "uniforms.h"

#include "orbiter.h"
#include "app.h"        // classe Application a deriver


class DrawInstanceID : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    DrawInstanceID( ) : App(1024, 640) {}
    
    int init( )
    {
        // charge un mesh
        Mesh mesh= read_mesh("data/cube.obj");
        if(mesh == Mesh::error()) return -1;
        
        Point pmin, pmax;
        mesh.bounds(pmin, pmax);
        m_camera.lookat(pmin - Vector(40, 0, 0), pmax + Vector(40, 0, 0));
        
        m_vertex_count= mesh.vertex_count();

        // cree les buffers et le vao
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        // buffer : positions + normals
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        
        size_t size= mesh.vertex_buffer_size() + mesh.normal_buffer_size();
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
        
        // transfere les positions des sommets
        size_t offset= 0;
        size= mesh.vertex_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, mesh.vertex_buffer());
        // configure l'attribut 0, vec3 position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset);
        glEnableVertexAttribArray(0);

        // transfere les normales des sommets
        offset= offset + size;
        size= mesh.normal_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, mesh.normal_buffer());
        // configure l'attribut 2, vec3 normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset);
        glEnableVertexAttribArray(2);
        
        //
        mesh.release();
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // shaders
        m_program= read_program("tutos/instanceID.glsl");
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
        release_program(m_program);
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_buffer);
        return 0;
    }
    
    int update( const float time, const float delta )
    {
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
        
        glBindVertexArray(m_vao);
        glUseProgram(m_program);
        
        Transform m= m_model;
        Transform v= m_camera.view();
        Transform p= m_camera.projection(window_width(), window_height(), 45);
        Transform mvp= p * v * m;
        Transform mv= v * m;
        
        program_uniform(m_program, "mvpMatrix", mvp);
        program_uniform(m_program, "normalMatrix", mv.normal());
        
        glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertex_count, 10);
        
        return 1;
    }

protected:
    Transform m_model;
    Orbiter m_camera;
    GLuint m_vao;
    GLuint m_buffer;
    GLuint m_program;
    int m_vertex_count;
};


int main( int argc, char **argv )
{
    DrawInstanceID tp;
    tp.run();
    
    return 0;
}
    
