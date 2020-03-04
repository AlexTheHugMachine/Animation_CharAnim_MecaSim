
//! \file tuto_stream.cpp utilisation de glBufferStorage() / glMapBufferRange()

#include <cstdlib>
#include <ctime>
#include <chrono>

#include "vec.h"
#include "mat.h"

#include "mesh.h"
#include "wavefront.h"

#include "program.h"
#include "uniforms.h"

#include "orbiter.h"
#include "app_time.h"


class DrawInstanceBuffer : public AppTime
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    DrawInstanceBuffer( ) : AppTime(1024, 640,  4,4) {}
    
    int init( )
    {
        // charge un mesh
        Mesh mesh= read_mesh("data/cube.obj");
        if(mesh == Mesh::error()) return -1;
        
        Point pmin, pmax;
        mesh.bounds(pmin, pmax);
        m_camera.lookat(pmin - Vector(40, 40, 40), pmax + Vector(40, 40, 40));
        
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
        size_t offset_positions= 0;
        size= mesh.vertex_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset_positions, size, mesh.vertex_buffer());
        // configure l'attribut 0, vec3 position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset_positions);
        glEnableVertexAttribArray(0);

        // transfere les normales des sommets
        size_t offset_normals= offset_positions + size;
        size= mesh.normal_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset_normals, size, mesh.normal_buffer());
        // configure l'attribut 2, vec3 normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset_normals);
        glEnableVertexAttribArray(2);
        
    // remarque : oui, c'est mal et la suite montre comment le faire correctement !!
        
        // instance buffer, position aleatoire des cubes...
        srand(time(nullptr));
        
        for(int i= 0; i < 1024*1024; i++)
        {
            float x= rand() % 256 - 128;
            float y= rand() % 256 - 128;
            float z= rand() % 256 - 128;
            
            m_positions.push_back(vec3(x *4, y *4, z *4));
        }
        
        
        m_instance_count= int(m_positions.size());
        
        printf("buffer %dKB\n", int(sizeof(vec3) * m_positions.size() / 1024));
        
        // cree et initialise le buffer d'instance
        // version openGL 3, buffer dynamique + glBufferSubData()
        glGenBuffers(1, &m_instance_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_positions.size(), m_positions.data(), GL_DYNAMIC_DRAW);

        // configure le vao pour l'attribut d'instance
        // configure l'attribut 1, vec3 instance_position
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, /* offset */ 0);
        glVertexAttribDivisor(1, 1);    // !! c'est la seule difference entre un attribut de sommet et un attribut d'instance !!
        glEnableVertexAttribArray(1);
        
        // openGL 4.4, buffer dynamique explicite + map persistant
        glGenBuffers(1, &m_instance_storage);
        glBindBuffer(GL_ARRAY_BUFFER, m_instance_storage);
        glBufferStorage(GL_ARRAY_BUFFER,  sizeof(vec3) * m_positions.size(), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
        
        m_storage= glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_positions.size(), 
            GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_PERSISTENT_BIT);
        if(m_storage == nullptr) 
            return -1;
        
        // cree et initialise un autre vao 
        glGenVertexArrays(1, &m_vao_storage);
        glBindVertexArray(m_vao_storage);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset_positions);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset_normals);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, m_instance_storage);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, /* offset */ 0);
        glVertexAttribDivisor(1, 1);    // !! c'est la seule difference entre un attribut de sommet et un attribut d'instance !!
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        
        //
        mesh.release();
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // shaders
        m_program= read_program("tutos/instance_buffer.glsl");
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
        glDeleteBuffers(1, &m_instance_buffer);
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
        
        static int mode= 0;
        if(key_state(SDLK_SPACE))
        {
            clear_key_state(SDLK_SPACE);
            mode= (mode + 1) % 5;
            printf("mode %d\n", mode);
            
            // alterne entre les 4 solutions de transfert...
        }
        
        // modifie le contenu du buffer d'instances
        if(mode == 0)
        {
            // strategie 1 :
            // recreer le buffer + transferer les donnees
            glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_positions.size(), m_positions.data(), GL_DYNAMIC_DRAW);
            
            glBindVertexArray(m_vao);
        }
        else if(mode == 1)
        {
            // strategie 2 :
            // transferer les donnees
            glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_positions.size(), m_positions.data());
            
            glBindVertexArray(m_vao);
        }
        else if(mode == 2)
        {
            // strategie 3 :
            // invalider les donnees + transfert
            glInvalidateBufferData(m_instance_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_positions.size(), m_positions.data());
            
            glBindVertexArray(m_vao);
        }
        else if(mode == 3)
        {
            // strategie 4 :
            // persistant map + flush + barrier
            
            std::chrono::high_resolution_clock::time_point copy_start= std::chrono::high_resolution_clock::now();
            memcpy(m_storage, m_positions.data(), sizeof(vec3) * m_positions.size());
            
            std::chrono::high_resolution_clock::time_point copy_stop= std::chrono::high_resolution_clock::now();
            auto copy_time= std::chrono::duration_cast<std::chrono::microseconds>(copy_stop - copy_start).count();
            printf("memcpy  %dK %02dus = %.2fK/s\n", int(sizeof(vec3) * m_positions.size()) / 1024, int(copy_time), float(sizeof(vec3) * m_positions.size() / 1024) / (float(copy_time) / 1000000));
            
            glBindBuffer(GL_ARRAY_BUFFER, m_instance_storage);            
            glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0,  sizeof(vec3) * m_positions.size());
            
            glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
            
            glBindVertexArray(m_vao_storage);
        }
        else
        {
            // pas de modifications
            glBindVertexArray(m_vao);
        }
        
        // draw
        glUseProgram(m_program);
        
        Transform m= m_model;
        Transform v= m_camera.view();
        Transform p= m_camera.projection(window_width(), window_height(), 45);
        Transform mvp= p * v * m;
        Transform mv= v * m;
        
        program_uniform(m_program, "mvpMatrix", mvp);
        program_uniform(m_program, "normalMatrix", mv.normal());
        
        glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertex_count, std::min(m_instance_count, 4096));
        
        return 1;
    }

protected:
    std::vector<vec3> m_positions;
    
    // solution openGL3
    GLuint m_vao;
    GLuint m_instance_buffer;
    // solution openGL4
    GLuint m_vao_storage;
    GLuint m_instance_storage;
    void *m_storage;

    Transform m_model;
    Orbiter m_camera;
    GLuint m_buffer;

    GLuint m_program;
    int m_vertex_count;
    int m_instance_count;
};


int main( int argc, char **argv )
{
    DrawInstanceBuffer tp;
    tp.run();
    
    return 0;
}
    
