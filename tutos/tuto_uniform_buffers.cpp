
//! \file tuto_uniform_buffer.cpp alignement des donnees / uniform buffers.

#include <cstddef>
#include <cassert>

#include <vector>

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
                printf(", array size %d", array_size);
            
            printf(", stride %d", array_stride);
            
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
    Context context= create_context(window, 4,3);       // openGL version 4.3
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

