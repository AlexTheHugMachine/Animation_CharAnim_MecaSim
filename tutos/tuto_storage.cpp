
#include <vector>

#include "window.h"
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
            if(params[1] == GL_FLOAT_MAT4) 
                printf(" %s, matrix stride %d", params[4] ? "row major" : "column major", params[3]);
            printf("\n");
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

