
#include <cfloat>
#include <cmath>

#include "app_time.h"

#include "vec.h"
#include "color.h"
#include "mat.h"

#include "mesh.h"
#include "wavefront.h"

#include "program.h"
#include "uniforms.h"

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


struct RT : public AppTime
{
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    RT( const char *filename ) : AppTime(1024, 640) 
    {
        m_mesh= read_mesh(filename);
    }
    
    int init( )
    {
        if(m_mesh == Mesh::error())
            return -1;
        
        Point pmin, pmax;
        m_mesh.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);
        
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
        
        // 
        struct triangle 
        {
            glsl::vec3 a;
            glsl::vec3 ab;
            glsl::vec3 ac;
        };
        
        std::vector<triangle> data;
        data.reserve(m_mesh.triangle_count());
        for(int i= 0; i < m_mesh.triangle_count(); i++)
        {
            TriangleData t= m_mesh.triangle(i);
            data.push_back( triangle { t.a, Point(t.b) - Point(t.a), Point(t.c) - Point(t.a) } );
        }
        
        // alloue le buffer
        // alloue au moins 1024 triangles, cf le shader
        if(data.size() < 1024)
            data.resize(1024);
        glBufferData(GL_UNIFORM_BUFFER, data.size() * sizeof(triangle), data.data(), GL_STATIC_READ);

        //
        m_program= read_program("tutos/M2/raytrace.glsl");
        program_print_errors(m_program);

        // associe l'uniform buffer a l'entree 0
        GLint index= glGetUniformBlockIndex(m_program, "triangleData");
        glUniformBlockBinding(m_program, index, 0);
        
        return 0;
    }
    
    int quit( )
    {
        return 0;
    }
    
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(key_state('f'))
        {
            clear_key_state('f');
            Point pmin, pmax;
            m_mesh.bounds(pmin, pmax);
            m_camera.lookat(pmin, pmax);        
        }
        
        // deplace la camera
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.move(mx);
        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        
        Transform m;
        Transform v= m_camera.view();
        Transform p= m_camera.projection(window_width(), window_height(), 45);
        Transform mvp= p * v * m;
        
        // config pipeline
        glBindVertexArray(m_vao);
        glUseProgram(m_program);
        
        // uniform buffer 0
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_buffer);
        program_uniform(m_program, "triangle_count", std::min((int) m_mesh.triangle_count(), 1024) );
        
        program_uniform(m_program, "mvpInvMatrix", mvp.inverse());
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        return 1;
    }
    
protected:
    Mesh m_mesh;
    Orbiter m_camera;

    GLuint m_program;
    GLuint m_vao;
    GLuint m_buffer;
};

    
int main( int argc, char **argv )
{
    const char *filename= "cornell.obj";
    if(argc > 1)
        filename= argv[1];
    
    RT app(filename);
    app.run();
    
    return 0;
}
