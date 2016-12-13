
//! \file tuto_storage.cpp utilisation de storage buffers.

#include <vector>
#include <cstddef>

#include "window.h"

#include "vec.h"
#include "program.h"


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
        GLint vcount= 0;
        {
            GLenum prop[]= { GL_NUM_ACTIVE_VARIABLES };
            glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, i, 1, prop, 1, NULL, &vcount);
        }
        
        // identifidants des variables 
        std::vector<GLint> variables(vcount);
        {
            GLenum prop[]= { GL_ACTIVE_VARIABLES };
            glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, i, 1, prop, vcount, NULL, &variables.front());
        }
        
        for(int k= 0; k < vcount; k++)
        {
            // organisation des variables dans le buffer
            GLenum props[]= { GL_OFFSET, GL_TYPE, GL_ARRAY_STRIDE, GL_MATRIX_STRIDE, GL_IS_ROW_MAJOR, GL_TOP_LEVEL_ARRAY_STRIDE };
            GLint params[sizeof(props) / sizeof(GLenum)];
            glGetProgramResourceiv(program, GL_BUFFER_VARIABLE, variables[k], sizeof(props) / sizeof(GLenum), props, sizeof(params) / sizeof(GLenum), NULL, params);
            
            // nom de la variable
            char vname[1024]= { 0 };
            glGetProgramResourceName(program, GL_BUFFER_VARIABLE, variables[k], sizeof(vname), NULL, vname);
            
            printf("    '%s %s': offset %d array stride %d, top level stride %d", glsl_string(params[1]), vname, params[0], params[2], params[5]);
            
            // organisation des matrices
            if(params[1] == GL_FLOAT_MAT4 || params[1] == GL_FLOAT_MAT3) 
                printf(" %s, matrix stride %d", params[4] ? "row major" : "column major", params[3]);
            printf("\n");
        }
    }
    
    return 0;
}


// application
GLuint program;

namespace glsl 
{
#ifdef _MSC_VER   // visual studio >= 2012 necessaire
# define ALIGN(...) __declspec(align(__VA_ARGS__))

#else   // gcc, clang, icc
# define ALIGN(...) __attribute__((aligned(__VA_ARGS__)))
#endif

    template < typename T >
    struct ALIGN(8) gvec2
    {
        ALIGN(4) T x, y;
    };
    
    typedef gvec2<float> vec2;
    typedef gvec2<int> ivec2;
    typedef gvec2<unsigned int> uvec2;
    typedef gvec2<int> bvec2;
    
    template < typename T >
    struct ALIGN(16) gvec3
    {
        ALIGN(4) T x, y, z;
    };
    
    typedef gvec3<float> vec3;
    typedef gvec3<int> ivec3;
    typedef gvec3<unsigned int> uvec3;
    typedef gvec3<int> bvec3;
    
    template < typename T >
    struct ALIGN(16) gvec4
    {
        ALIGN(4) T x, y, z, w;
    };
    
    typedef gvec4<float> vec4;
    typedef gvec4<int> ivec4;
    typedef gvec4<unsigned int> uvec4;
    typedef gvec4<int> bvec4;
}

int init( )
{
    // compile le shader program, le program est selectionne
    program= read_program("tutos/storage.glsl");
    program_print_errors(program);
    
    print_storage(program);


    struct toto
    {
        vec3 a;
        vec3 ab;
        vec3 ac;
    };
    
    printf("a %d\n", (int) offsetof(toto, a));
    printf("a.x %d\n", (int) offsetof(toto, a.x));
    printf("a.y %d\n", (int) offsetof(toto, a.y));
    printf("a.z %d\n", (int) offsetof(toto, a.z));
    printf("ab %d\n", (int) offsetof(toto, ab));
    printf("ac %d\n", (int) offsetof(toto, ac));
    
    struct titi
    {
        glsl::vec3 a;
        glsl::vec3 ab;
        glsl::vec3 ac;
    };
    
    printf("a %d\n", (int) offsetof(titi, a));
    printf("a.x %d\n", (int) offsetof(titi, a.x));
    printf("a.y %d\n", (int) offsetof(titi, a.y));
    printf("a.z %d\n", (int) offsetof(titi, a.z));
    printf("ab %d\n", (int) offsetof(titi, ab));
    printf("ac %d\n", (int) offsetof(titi, ac));
    
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

