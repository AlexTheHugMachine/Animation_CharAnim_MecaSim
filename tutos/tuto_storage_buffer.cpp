
//! \file tuto_storage_buffer.cpp  exemple direct d'utilisation d'un storage buffer.  le vertex shader recupere les attributs des sommets dans un storage buffer sans utiliser de vao.

#include "app.h"

#include "vec.h"
#include "mat.h"

#include "program.h"
#include "uniforms.h"
#include "texture.h"

#include "mesh.h"
#include "wavefront.h"

#include "orbiter.h"


// cf tuto_storage
namespace glsl 
{
    template < typename T >
    struct alignas(8) gvec2
    {
        alignas(4) T x, y;
        
        gvec2( ) {}
        gvec2( const vec2& v ) : x(v.x), y(v.y) {}
    };
    
    typedef gvec2<float> vec2;
    typedef gvec2<int> ivec2;
    typedef gvec2<unsigned int> uvec2;
    typedef gvec2<int> bvec2;
    
    template < typename T >
    struct alignas(16) gvec3
    {
        alignas(4) T x, y, z;
        
        gvec3( ) {}
        gvec3( const vec3& v ) : x(v.x), y(v.y), z(v.z) {}
        gvec3( const Point& v ) : x(v.x), y(v.y), z(v.z) {}
        gvec3( const Vector& v ) : x(v.x), y(v.y), z(v.z) {}
    };
    
    typedef gvec3<float> vec3;
    typedef gvec3<int> ivec3;
    typedef gvec3<unsigned int> uvec3;
    typedef gvec3<int> bvec3;
    
    template < typename T >
    struct alignas(16) gvec4
    {
        alignas(4) T x, y, z, w;
        
        gvec4( ) {}
        gvec4( const vec4& v ) : x(v.x), y(v.y), z(v.z), w(v.w) {}
    };
    
    typedef gvec4<float> vec4;
    typedef gvec4<int> ivec4;
    typedef gvec4<unsigned int> uvec4;
    typedef gvec4<int> bvec4;
}


class StorageBuffer : public App
{
public:
    // application openGL 4.3
    StorageBuffer( ) : App(1024, 640,  4, 3) {}
    
    int init( )
    {
        m_mesh= read_mesh("data/cube.obj");
        
    #if 1
        // construit le storage buffer contenant les positions, les normales et les texcoords, en utilisant les types alignes
        struct vertex
        {
            glsl::vec3 position;
            glsl::vec3 normal;
            glsl::vec2 texcoord;
            
            vertex( ) : position(), normal(), texcoord() {}
        };
        
    #else
        // ou a la main
        struct vertex
        {
            vec3 position;      // vec3 aligne sur 4 float
            float pad0;
            vec3 normal;
            float pad1;
            vec2 texcoord;      // vec2 aligne sur 2 float
            
            float pad2;
            float pad3;      // donc la taille totale de la structure doit etre un mulitple de 4 floats
            
            vertex( ) : position(), normal(), texcoord() {}
        };
    #endif
        
        
        // recupere les attributs du mesh
        std::vector<vertex> data(m_mesh.vertex_count());
        for(int i= 0; i < m_mesh.vertex_count(); i++)
        {
            data[i].position= m_mesh.positions().at(i);
            data[i].normal= m_mesh.normals().at(i);
            data[i].texcoord= m_mesh.texcoords().at(i);
        }
        
        // vao par defaut
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        // storage buffer
        glGenBuffers(1, &m_buffer);
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertex) * data.size(), data.data(), GL_STREAM_READ);
        
        // 
        m_program= read_program("tutos/tuto_storage_buffer.glsl");
        program_print_errors(m_program);
        
        //
        Point pmin, pmax;
        m_mesh.bounds(pmin, pmax);
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
        m_mesh.release();
        release_program(m_program);
        glDeleteTextures(1, &m_texture);
        glDeleteBuffers(1, &m_buffer);
        glDeleteVertexArrays(1, &m_vao);
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

        Transform model= RotationY(global_time() / 20);
        Transform view= m_camera.view();
        Transform projection= m_camera.projection(window_width(), window_height(), 45);
        Transform mv= view * model;
        Transform mvp= projection * mv;
        
        program_uniform(m_program, "mvMatrix", mv);
        program_uniform(m_program, "normalMatrix", mv.normal());
        program_uniform(m_program, "mvpMatrix", mvp);
        
        program_use_texture(m_program, "diffuse_color", 0, m_texture, 0);
        
        // selectionne le buffer comme storage buffer, numero 0
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_buffer);
        
        glDrawArrays(GL_TRIANGLES, 0, m_mesh.vertex_count());
        
        return 1;
    }

protected:
    Mesh m_mesh;
    Orbiter m_camera;

    GLuint m_vao;
    GLuint m_buffer;
    GLuint m_program;
    GLuint m_texture;
};


int main( int argc, char **argv )
{
    StorageBuffer app;
    app.run();
    
    return 0;
}
