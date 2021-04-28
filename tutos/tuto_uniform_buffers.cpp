
//! \file tuto_uniform_buffers.cpp alignement des donnees / uniform buffers.

#include <cstddef>
#include <cassert>

#include <vector>

#include "window.h"

#include "vec.h"
#include "color.h"
#include "program.h"


namespace glsl 
{
    // type de base alignes sur 4 octets
    template < typename T >
    struct alignas(4) gscalar
    { 
        alignas(4) T x;
        
        gscalar( ) : x(T()) {}
        gscalar( const T& v ) : x(v) {}
        gscalar& operator= ( const T& v ) { x= v; return *this; }
        operator T ( ) { return x; }
    };

    typedef gscalar<float> gfloat;
    typedef gscalar<int> gint;
    typedef gscalar<unsigned int> guint;
    typedef gscalar<bool> gbool;
    
    // vec2, alignes sur 2 * alignement type de base du vecteur
    template < typename T >
    struct alignas(8) gvec2
    {
        alignas(4) T x, y;
        
        gvec2( ) {}
        gvec2( const gvec2<T>& v ) : x(v.x), y(v.y) {}
        gvec2( const ::vec2& v ) : x(v.x), y(v.y) {}
        gvec2& operator= ( const gvec2<T>& v ) { x= v.x; y= v.y; return *this; }
        gvec2& operator= ( const ::vec2& v ) { x= v.x; y= v.y; return *this; }
        operator ::vec2 ( ) { return ::vec2(float(x), float(y)); }
    };
    
    typedef gvec2<float> vec2;
    typedef gvec2<int> ivec2;
    typedef gvec2<unsigned int> uvec2;
    typedef gvec2<int> bvec2;
    
    // vec3, alignes sur 4 * alignement type de base du vecteur
    template < typename T >
    struct alignas(16) gvec3
    {
        alignas(4) T x, y, z;
        
        gvec3( ) {}
        gvec3( const gvec3<T>& v ) : x(v.x), y(v.y), z(v.z) {}
        gvec3( const ::vec3& v ) : x(v.x), y(v.y), z(v.z) {}
        gvec3( const Point& v ) : x(v.x), y(v.y), z(v.z) {}
        gvec3( const Vector& v ) : x(v.x), y(v.y), z(v.z) {}
        gvec3& operator= ( const gvec3<T>& v ) { x= v.x; y= v.y; z= v.z; return *this; }
        gvec3& operator= ( const ::vec3& v ) { x= v.x; y= v.y; z= v.z; return *this; }
        gvec3& operator= ( const Point& v ) { x= v.x; y= v.y; z= v.z; return *this; }
        gvec3& operator= ( const Vector& v ) { x= v.x; y= v.y; z= v.z; return *this; }
        operator ::vec3 ( ) { return ::vec3(float(x), float(y), float(y)); }
    };
    
    typedef gvec3<float> vec3;
    typedef gvec3<int> ivec3;
    typedef gvec3<unsigned int> uvec3;
    typedef gvec3<int> bvec3;
    
    // vec4, alignes sur 4 * alignement type de base du vecteur
    template < typename T >
    struct alignas(16) gvec4
    {
        alignas(4) T x, y, z, w;
        
        gvec4( ) {}
        gvec4( const gvec4<T>& v ) : x(v.x), y(v.y), z(v.z), w(v.w) {}
        gvec4( const ::vec4& v ) : x(v.x), y(v.y), z(v.z), w(v.w) {}
        gvec4& operator= ( const gvec4<T>& v ) { x(v.x), y(v.y), z(v.z), w(v.w) ; return *this; }
        gvec4& operator= ( const ::vec4& v ) { x(v.x), y(v.y), z(v.z), w(v.w) ; return *this; }
        gvec4& operator= ( const Color& c ) { x= c.r; y= c.g; z= c.b; w= c.a; return *this; }
        operator ::vec4 ( ) { return ::vec4(float(x), float(y), float(y), float(w)); }
    };
    
    typedef gvec4<float> vec4;
    typedef gvec4<int> ivec4;
    typedef gvec4<unsigned int> uvec4;
    typedef gvec4<int> bvec4;
    
    // alignement different pour les elements d'un tableau : 16 octets, meme pour les types plus petits comme les float, int, bool
    namespace array
    {
        template < typename T >
        struct alignas(16) gscalar
        { 
            T x;
            
            gscalar( ) : x(T()) {}
            gscalar( const T& v ) : x(v) {}
            gscalar& operator= ( const T& v ) { x= v; return *this; }
            operator T ( ) { return x; }
        };
    
        typedef gscalar<float> gfloat;
        typedef gscalar<int> gint;
        typedef gscalar<unsigned int> guint;
        typedef gscalar<bool> gbool;
        
        template < typename T >
        struct alignas(16) gvec2
        {
            alignas(4) T x, y;
            
            gvec2( ) {}
            gvec2( const gvec2<T>& v ) : x(v.x), y(v.y) {}
            gvec2( const ::vec2& v ) : x(v.x), y(v.y) {}
            gvec2& operator= ( const gvec2<T>& v ) { x= v.x; y= v.y; return *this; }
            gvec2& operator= ( const ::vec2& v ) { x= v.x; y= v.y; return *this; }
            operator ::vec2 ( ) { return ::vec2(float(x), float(y)); }
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
            gvec3( const gvec3<T>& v ) : x(v.x), y(v.y), z(v.z) {}
            gvec3( const ::vec3& v ) : x(v.x), y(v.y), z(v.z) {}
            gvec3( const Point& v ) : x(v.x), y(v.y), z(v.z) {}
            gvec3( const Vector& v ) : x(v.x), y(v.y), z(v.z) {}
            gvec3& operator= ( const gvec3<T>& v ) { x= v.x; y= v.y; z= v.z; return *this; }
            gvec3& operator= ( const ::vec3& v ) { x= v.x; y= v.y; z= v.z; return *this; }
            gvec3& operator= ( const Point& v ) { x= v.x; y= v.y; z= v.z; return *this; }
            gvec3& operator= ( const Vector& v ) { x= v.x; y= v.y; z= v.z; return *this; }
            operator ::vec3 ( ) { return ::vec3(float(x), float(y), float(y)); }
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
            gvec4( const gvec4<T>& v ) : x(v.x), y(v.y), z(v.z), w(v.w) {}
            gvec4( const ::vec4& v ) : x(v.x), y(v.y), z(v.z), w(v.w) {}
            gvec4& operator= ( const gvec4<T>& v ) { x(v.x), y(v.y), z(v.z), w(v.w) ; return *this; }
            gvec4& operator= ( const ::vec4& v ) { x(v.x), y(v.y), z(v.z), w(v.w) ; return *this; }
            gvec4& operator= ( const Color& c ) { x= c.r; y= c.g; z= c.b; w= c.a; return *this; }
            operator ::vec4 ( ) { return ::vec4(float(x), float(y), float(y), float(w)); }
        };
        
        typedef gvec4<float> vec4;
        typedef gvec4<int> ivec4;
        typedef gvec4<unsigned int> uvec4;
        typedef gvec4<int> bvec4;
    }
}



// utilitaire : renvoie la chaine de caracteres pour un type glsl.
const char *glsl_string( const GLenum type )
{
    switch(type)
    {
        case GL_BOOL:
            return "bool";
        case GL_UNSIGNED_INT:
            return "uint";
        case GL_INT:
            return "int";
        case GL_FLOAT:
            return "float";
        case GL_FLOAT_VEC2:
            return "vec2";
        case GL_FLOAT_VEC3:
            return "vec3";
        case GL_FLOAT_VEC4:
            return "vec4";
        case GL_FLOAT_MAT3:
            return "mat3";
        case GL_FLOAT_MAT4:
            return "mat4";

        default:
            return "";
    }
}

int print_uniform( const GLuint program )
{
    if(program == 0)
    {
        printf("[error] program 0, no buffers...\n");
        return -1;
    }
    
    // recupere le nombre d'uniform buffers
    GLint buffer_count= 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &buffer_count);
    
    for(int i= 0; i < buffer_count; i++)
    {
        // recupere le nom du block et son indice
        char bname[1024]= { 0 };
        glGetActiveUniformBlockName(program, i, sizeof(bname), nullptr, bname);
        
        GLint binding= 0;
        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_BINDING, &binding);
        
        printf("  uniform '%s' binding %d\n", bname, binding);
        
        GLint variable_count= 0;
        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &variable_count);
        
        std::vector<GLint> variables(variable_count);
        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, variables.data());
        for(int k= 0; k < variable_count; k++)
        {
            // recupere chaque info... une variable a la fois, 
            GLint glsl_type= 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k], GL_UNIFORM_TYPE, &glsl_type);
            GLint offset= 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k], GL_UNIFORM_OFFSET, &offset);
            
            GLint array_size= 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k], GL_UNIFORM_SIZE, &array_size);
            GLint array_stride= 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k], GL_UNIFORM_ARRAY_STRIDE, &array_stride);
            
            GLint matrix_stride= 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k], GL_UNIFORM_MATRIX_STRIDE, &matrix_stride);
            GLint matrix_row_major= 0;
            glGetActiveUniformsiv(program, 1, (GLuint *) &variables[k], GL_UNIFORM_IS_ROW_MAJOR, &matrix_row_major);
            
            // nom de la variable
            char vname[1024]= { 0 };
            glGetActiveUniformName(program, variables[k], sizeof(vname), nullptr, vname);
            
            printf("    '%s %s': offset %d", glsl_string(glsl_type), vname, offset);
            if(array_size > 1)
                printf(", array size %d, stride %d", array_size, array_stride);
            
            // organisation des matrices
            if(glsl_type == GL_FLOAT_MAT4 || glsl_type == GL_FLOAT_MAT3)
                printf(", %s, matrix stride %d", matrix_row_major ? "row major" : "column major", matrix_stride);
            
            printf("\n");
        }
        
        GLint buffer_size= 0;
        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &buffer_size);
        printf("  buffer size %d\n\n", buffer_size);
    }
    
    return 0;
}

// application
GLuint program;

int init( )
{
    // compile le shader program, le program est selectionne
    program= read_program("tutos/uniform.glsl");
    program_print_errors(program);
    
    // affiche l'organisation memoire des uniforms
    print_uniform(program);
    
    struct buffer
    {
        glsl::vec2 a;
        glsl::vec3 b;
        glsl::vec4 c;
        glsl::array::vec2 d[3];

        //~ vec2 a;     // align 4
        //~ float pad0;
        //~ float pad1;
        //~ vec3 b;     // align 16
        //~ float pad2;
        //~ vec4 c;     // align 16
        //~ struct { vec2 v; float pad0; float pad1; } d[3];    // d[i].v align 16
        
        //~ vec2 a;
        //~ vec3 b;
        //~ vec4 c;
        //~ vec2 d[3];
    };
    
    printf("a %ld\n",   offsetof(buffer, a));
    printf("a.x %ld\n", offsetof(buffer, a.x));
    printf("a.y %ld\n", offsetof(buffer, a.y));
    
    printf("b %ld\n",   offsetof(buffer, b));
    printf("b.x %ld\n", offsetof(buffer, b.x));
    printf("b.y %ld\n", offsetof(buffer, b.y));
    printf("b.z %ld\n", offsetof(buffer, b.z));
    
    printf("c %ld\n",   offsetof(buffer, c));
    printf("c.x %ld\n", offsetof(buffer, c.x));
    printf("c.y %ld\n", offsetof(buffer, c.y));
    printf("c.z %ld\n", offsetof(buffer, c.z));
    printf("c.w %ld\n", offsetof(buffer, c.w));
    
    printf("d %ld\n",    offsetof(buffer, d));
    printf("d[0] %ld\n", offsetof(buffer, d[0]));
    printf("d[1] %ld\n", offsetof(buffer, d[1]));
    printf("d[2] %ld\n", offsetof(buffer, d[2]));
    
    buffer B;
    B.a= vec2(1, 2);
    B.d[0]= vec2(3, 4);
    B.d[1]= B.d[0];
    
    //~ vec2 d0= B.d[0];
    vec2 d0= B.d[1];
    printf("%f %f\n", d0.x, d0.y);
    return 0;
}

int quit( )
{
    release_program(program);
    return 0;
}


int main( int argc, char **argv )
{
    // etape 1 : creer la fenetre
    Window window= create_window(1024, 640);
    if(window == nullptr)
        return 1;

    // etape 2 : creer un contexte opengl pour pouvoir dessiner
    Context context= create_context(window, 3,3);       // openGL version 3.3
    if(context == nullptr)
        return 1;

    // etape 3 : creation des objets
    if(init() < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }

    // etape 5 : nettoyage
    quit();
    release_context(context);
    release_window(window);
    return 0;
}

