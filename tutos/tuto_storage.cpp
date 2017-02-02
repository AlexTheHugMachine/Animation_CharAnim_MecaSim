
//! \file tuto_storage.cpp alignement des donnees / storage buffers.

#include <cstddef>
#include <cassert>

#include <vector>

#include "window.h"

#include "vec.h"
#include "program.h"


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
        case GL_FLOAT_MAT4:
            return "mat4";

        default:
            return "";
    }
}

int print_storage( const GLuint program )
{
    if(program == 0)
    {
        printf("[error] program 0, no storage buffers...\n");
        return -1;
    }
    
    // recupere le nombre de storage buffers
    GLint buffer_count= 0;
    glGetProgramInterfaceiv(program, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &buffer_count);
    if(buffer_count == 0)
        return 0;
    
    for(int i= 0; i < buffer_count; i++)
    {
        // recupere le nom du storage buffer
        char bname[1024]= { 0 };
        glGetProgramResourceName(program, GL_SHADER_STORAGE_BLOCK, i, sizeof(bname), NULL, bname);
        
        // et le binding
        GLint binding= 0;
        {
            GLenum prop[]= { GL_BUFFER_BINDING };
            glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, i, 1, prop, 1, NULL, &binding);
        }

        printf("  buffer '%s' binding %d\n", bname, binding);
        
        // nombre de variables declarees
        GLint variable_count= 0;
        {
            GLenum prop[]= { GL_NUM_ACTIVE_VARIABLES };
            glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, i, 1, prop, 1, NULL, &variable_count);
        }
        
        // identifidants des variables 
        std::vector<GLint> variables(variable_count);
        {
            GLenum prop[]= { GL_ACTIVE_VARIABLES };
            glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, i, 1, prop, variable_count, NULL, variables.data());
        }
        
        for(int k= 0; k < variable_count; k++)
        {
            // organisation des variables dans le buffer
            GLenum props[]= { GL_OFFSET, GL_TYPE, GL_ARRAY_SIZE, GL_ARRAY_STRIDE, GL_MATRIX_STRIDE, GL_IS_ROW_MAJOR, GL_TOP_LEVEL_ARRAY_STRIDE };
            const int size= sizeof(props) / sizeof(GLenum);
            
            GLint params[size];
            glGetProgramResourceiv(program, GL_BUFFER_VARIABLE, variables[k], size, props, size, NULL, params);
            
            // nom de la variable
            char vname[1024]= { 0 };
            glGetProgramResourceName(program, GL_BUFFER_VARIABLE, variables[k], sizeof(vname), NULL, vname);
            
            printf("    '%s %s': offset %d", glsl_string(params[1]), vname, params[0]);
            if(params[2] > 1)
                printf(", array size %d", params[2]);
            
            printf(", stride %d", params[3]);
            
            // organisation des matrices
            if(params[1] == GL_FLOAT_MAT4 || params[1] == GL_FLOAT_MAT3) 
                printf(", %s, matrix stride %d", params[5] ? "row major" : "column major", params[4]);
            
            printf(", top level stride %d\n", params[6]);
        }
    }
    
    return 0;
}


// application
GLuint program;

int init( )
{
    // compile le shader program, le program est selectionne
    program= read_program("tutos/storage.glsl");
    program_print_errors(program);
    
    print_storage(program);


    struct TriangleCPU
    {
        vec3 a;
        vec3 b;
        vec3 c;
    };
    
    printf("cpu:\n");
    printf("  a %d\n", (int) offsetof(TriangleCPU, a));
    printf("    a.x %d\n", (int) offsetof(TriangleCPU, a.x));
    printf("    a.y %d\n", (int) offsetof(TriangleCPU, a.y));
    printf("    a.z %d\n", (int) offsetof(TriangleCPU, a.z));
    printf("  b %d\n", (int) offsetof(TriangleCPU, b));
    printf("  c %d\n", (int) offsetof(TriangleCPU, c));
    printf("= %dB\n", (int) sizeof(TriangleCPU));

    
    struct TriangleGLSL
    {
        glsl::vec3 a;
        glsl::vec3 b;
        glsl::vec3 c;
        
        TriangleGLSL( const Point& _a, const Point& _b, const Point& _c ) : a(_a), b(_b), c(_c) {}
        TriangleGLSL( const vec3& _a, const vec3& _b, const vec3& _c ) : a(_a), b(_b), c(_c) {}
    };
    
    printf("glsl:\n");
    printf("  a %d\n", (int) offsetof(TriangleGLSL, a));
    printf("    a.x %d\n", (int) offsetof(TriangleGLSL, a.x));
    printf("    a.y %d\n", (int) offsetof(TriangleGLSL, a.y));
    printf("    a.z %d\n", (int) offsetof(TriangleGLSL, a.z));
    printf("  b %d\n", (int) offsetof(TriangleGLSL, b));
    printf("  c %d\n", (int) offsetof(TriangleGLSL, c));    
    printf("= %dB\n", (int) sizeof(TriangleGLSL));
    
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
    if(window == NULL)
        return 1;

    // etape 2 : creer un contexte opengl pour pouvoir dessiner
    Context context= create_context(window, 4,3);       // openGL version 4.3
    if(context == NULL)
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

