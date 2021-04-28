
//! \file tuto_vertex_compute.cpp  exemple direct.

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
        gvec2( const ::vec2& v ) : x(v.x), y(v.y) {}
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
        gvec3( const ::vec3& v ) : x(v.x), y(v.y), z(v.z) {}
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
        gvec4( const ::vec4& v ) : x(v.x), y(v.y), z(v.z), w(v.w) {}
    };
    
    typedef gvec4<float> vec4;
    typedef gvec4<int> ivec4;
    typedef gvec4<unsigned int> uvec4;
    typedef gvec4<int> bvec4;
}


class VertexCompute : public App
{
public:
    // application openGL 4.3
    VertexCompute( ) : App(1024, 640,  4,3) {}
    
    int init( )
    {
        m_mesh= read_mesh("data/bigguy.obj");
        printf("  positions %d\n", m_mesh.vertex_count());
        printf("  triangles %d\n", m_mesh.triangle_count());
        
        // construit le storage buffer contenant les positions, les normales et les texcoords, en utilisant les types alignes
        struct vertex
        {
            glsl::vec3 position;
        };
        
        // recupere les attributs du mesh
        std::vector<vertex> data(m_mesh.vertex_count());
        for(int i= 0; i < m_mesh.vertex_count(); i++)
            data[i].position= m_mesh.positions().at(i);
        
        // vao par defaut, pas d'attribut. les positions des sommets sont dans le storage buffer...
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        // storage buffers
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertex) * data.size(), data.data(), GL_STREAM_READ);
        
        glGenBuffers(1, &m_transformed_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_transformed_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4) * data.size(), std::vector<vec4>(data.size(), vec4()).data(), GL_STREAM_COPY);
        
        // 
        m_program= read_program("tutos/pipeline_compute.glsl");
        program_print_errors(m_program);
        
        // compute shader
        m_compute_program= read_program("tutos/vertex_compute.glsl");
        program_print_errors(m_compute_program);
        
        // recupere le nombre de threads de chaque groupe du compute shader
        GLint threads[3]= { };
        glGetProgramiv(m_compute_program, GL_COMPUTE_WORK_GROUP_SIZE, threads);
        printf("threads / group x %d, y %d, z %d\n", threads[0], threads[1], threads[2]);
        m_compute_threads= threads[0];
        
        // nombre de groupes de threads a executer pour transformer les sommets du mesh
        m_compute_groups= m_mesh.vertex_count() / m_compute_threads;
        if(m_mesh.vertex_count() % m_compute_threads)
            m_compute_groups= m_compute_groups +1;
        
        printf("groups %d= %d threads\n", m_compute_groups, m_compute_groups*m_compute_threads);
        
        //
        Point pmin, pmax;
        m_mesh.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      // ne dessine que les aretes des triangles
        glLineWidth(2);
        
        return 0;   // ras, pas d'erreur
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        m_mesh.release();
        release_program(m_program);
        release_program(m_compute_program);
        glDeleteBuffers(1, &m_buffer);
        glDeleteBuffers(1, &m_transformed_buffer);
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

        // selectionne les buffers
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_transformed_buffer);
        
        // etape 1 : utilise le compute shader pour transformer les positions des sommets du mesh.
        glUseProgram(m_compute_program);
        
        // uniforms du compute shader
        Transform model= RotationY(global_time() / 60);
        Transform view= m_camera.view();
        Transform projection= m_camera.projection(window_width(), window_height(), 45);
        Transform mvp= projection * view * model;
        
        program_uniform(m_compute_program, "mvpMatrix", mvp);
        
        // go !!
        glDispatchCompute(m_compute_groups, 1, 1);
        
        // etape 2 : synchronisation, attendre les resultats du compute shader
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        
        // etape 3 : utilise les sommets transformes (par le compute shader) pour afficher le mesh.
        glBindVertexArray(m_vao);
        glUseProgram(m_program);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_transformed_buffer);

        glDrawArrays(GL_TRIANGLES, 0, m_mesh.vertex_count());

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

        return 1;
    }

protected:
    Mesh m_mesh;
    Orbiter m_camera;

    GLuint m_vao;
    GLuint m_buffer;
    GLuint m_transformed_buffer;
    GLuint m_program;
    GLuint m_compute_program;
    int m_compute_threads;
    int m_compute_groups;
};


int main( int argc, char **argv )
{
    VertexCompute app;
    app.run();
    
    return 0;
}
