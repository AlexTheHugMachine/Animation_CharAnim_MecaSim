
//! \file tuto3GL_reflect.cpp afficher les uniforms et les attributs utilises par un program.

#include "window.h"
#include "program.h"
#include "mesh.h"
#include "wavefront.h"

#include <stdio.h>

// identifiant du shader program
GLuint program;

// identifiant du vertex array object
GLuint vao;

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

// utilitaire : affiche la valeur d'un uniform.
void print_uniform_value( const GLuint program, const int index, const char *name, const GLenum type, const GLint size )
{
    if(program == 0)
        return;
    if(index  < 0)
        return;

    int value= 0;
    float values[16]= { };

/* les tableaux occuppent plusieurs uniforms places les uns apres les autres, donc :
        for(int i= 0; i < size; i++)
            glGetActiveUniform(program, location + i, ...)
    permet de recuperer toutes les cellules du tableau...
 */

    // limite l'affichage d'un tableau a quelques valeurs
    int n= std::min(10, size);
    for(int i= 0; i < n; i++)
    {
        if(size > 1)
            printf("[%02d]  ", i);
        else
            printf("      ");

        switch(type)
        {
            case GL_BOOL:
                glGetUniformiv(program, index +i, &value);
                printf("value= %s\n", value ? "true" : "false");
                break;
            case GL_INT:
                glGetUniformiv(program, index +i, &value);
                printf("value= %d\n", value);
                break;
            case GL_FLOAT:
                glGetUniformfv(program, index +i, values);
                printf("value= %f\n", values[0]);
                break;
            case GL_FLOAT_VEC2:
                glGetUniformfv(program, index +i, values);
                printf("values= %f %f\n", values[0], values[1]);
                break;
            case GL_FLOAT_VEC3:
                glGetUniformfv(program, index +i, values);
                printf("values= %f %f %f\n", values[0], values[1], values[2]);
                break;
            case GL_FLOAT_VEC4:
                glGetUniformfv(program, index +i, values);
                printf("values= %f %f %f %f\n", values[0], values[1], values[2], values[3]);
                break;
            case GL_FLOAT_MAT4:
                glGetUniformfv(program, index +i, values);
                printf("values=\n");
                for(int r= 0; r < 4; r++)
                {
                    printf("        ");
                    for(int c= 0; c < 4; c++)
                        printf("%f ", values[c*4 + r]);
                    printf("\n");
                }
                break;

            default:
                printf("    uniform '%s', type %x, type unknown\n", name, type);
        }
    }
    
    // indiquer que le tableau a ete tronque
    if(n < size)
        printf("...   \n");
}

// utilitaire : affiche les uniforms utilises par un program
int print_uniforms( const GLuint program )
{
    if(program == 0)
    {
        printf("[error] program 0, no uniforms...\n");
        return -1;
    }

    // recuperer le nombre total d'uniforms
    GLint n= 0;
    glGetProgramiv(program,  GL_ACTIVE_UNIFORMS, &n);
    if(n == 0)
    {
        printf("program %u: no uniforms...\n", program);
        return 0;
    }

    // recuperer la longueur max occuppee par un nom d'uniform
    GLint length_max= 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length_max);

    // allouer une chaine de caractere
    char *name= new char [length_max];

    printf("program %u:\n", program);
    // recuperer les infos de chaque uniform
    for(int index= 0; index < n; index++)
    {
        GLint glsl_size;
        GLenum glsl_type;
        glGetActiveUniform(program, index, length_max, NULL, &glsl_size, &glsl_type, name);
        // et son identifiant
        GLint location= glGetUniformLocation(program, name);

        printf("  uniform %i '%s': location %d, type %s (0x%x), array_size %d\n", index, name, location,
            glsl_string(glsl_type), glsl_type, glsl_size);
        
        print_uniform_value(program, index, name, glsl_type, glsl_size);
    }

    delete [] name;
    return 0;
}


// utilitaire : affiche tous les attributs utilises par un program, et eventuellement a quel buffer ils sont associes
int print_attribs( const GLuint program, const GLuint vao= 0 )
{
    if(program == 0)
    {
        printf("[error] program 0, no attributes...\n");
        return -1;
    }

    // recuperer le nombre total d'attributs
    GLint n= 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &n);
    if(n == 0)
    {
        printf("program %u: no attributes...\n", program);
        return 0;
    }

    // recuperer la longueur max occuppee par un nom d'attribut
    GLint length_max= 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length_max);

    // allouer une chaine de caractere
    char *name= new char [length_max];

    printf("program %u:\n", program);
    // recuperer les infos de chaque attribut
    for(int index= 0; index < n; index++)
    {
        GLint glsl_size;
        GLenum glsl_type;
        glGetActiveAttrib(program, index, length_max, NULL, &glsl_size, &glsl_type, name);
        // et son identifiant
        GLint location= glGetAttribLocation(program, name);

        printf("  attribute %d '%s': location %d, type %s (0x%x), array_size %d\n", index, name, location,
            glsl_string(glsl_type), glsl_type, glsl_size);
        
        if(vao > 0)
        {
            GLint buffer= 0; 
            glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer);
            if(buffer > 0)
                printf("      buffer %d\n", buffer);
            else
                printf("      no buffer\n");
        }
    }

    delete [] name;
    return 0;
}


int init( )
{
    // charge 12 triangles, soit un cube...
    Mesh cube= read_mesh("data/flat_bbox.obj");
    printf("  %d positions\n", cube.vertex_count());

    // compile le shader program, le program est selectionne
    program= read_program("tutos/tuto3GL.glsl");

    // affiche les uniforms et leurs valeurs par defaut
    print_uniforms(program);
    // affiche les attributs
    //~ print_attribs(program);
    
    // transfere les 36 positions dans le tableau declare par le vertex shader
    // etape 1 : recuperer l'identifiant de l'uniform
    GLint location= glGetUniformLocation(program, "positions"); // uniform vec3 positions[36];
    // etape 2 : modifier sa valeur
    glUniform3fv(location, cube.vertex_count(), cube.vertex_buffer());

    // mesh n'est plus necessaire
    cube.release();

    // creer un vertex array object
    glGenVertexArrays(1, &vao);

    // afficher les uniforms apres l'affectation...
    print_uniforms(program);
    // afficher les attributs... et leurs buffers
    print_attribs(program, vao);

    // etat openGL par defaut
    glClearColor(0.2, 0.2, 0.2, 1);     // definir la couleur par defaut
    glClearDepth(1.f);                  // profondeur par defaut

    glDepthFunc(GL_LESS);               // ztest, conserver l'intersection la plus proche de la camera
    glEnable(GL_DEPTH_TEST);            // activer le ztest

    return 0;
}

int quit( )
{
    release_program(program);
    glDeleteVertexArrays(1, &vao);
    return 0;
}


// affichage
int draw( )
{
    // effacer la fenetre : copier la couleur par defaut dans tous les pixels de la fenetre
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // "effacer"

    // configurer le pipeline, selectionner le vertex array a utiliser
    glBindVertexArray(vao);

    // configurer le pipeline, selectionner le shader program a utiliser
    glUseProgram(program);

    // modifier les valeurs des parametres uniforms du shader program
    GLint location;
    location= glGetUniformLocation(program, "color");   // declare dans le fragment shader
    glUniform4f(location, 1, 0.5, 0, 1);                // vec4 color;

/* modifiez la valeur de time, declare dans le vertex shader pour modifier la position du cube dans le repere du monde,
    en fonction du temps. vous pouvez utiliser la fonction mod ou modf pour creer un mouvement cyclique

    utilisez la fonction SDL_GetTicks( ), pour connaitre l'heure exprimee en milli secondes

    float time= SDL_GetTicks();
    location= glGetUniformLocation(program, "time");
    glUniform1f(location, time);
 */

/* utilisez print_uniforms(program) pour verifier les valeurs des parametres uniforms
    remarque: eventuellement pas tres lisible...
        modifiez print_uniform( ) pour n'afficher un message que si l'uniform est egal a 0
 */

    // dessiner 12 triangles, indices gl_VertexID de 0 a 36
    glDrawArrays(GL_TRIANGLES, 0, 36);

    return 1;   // on continue, renvoyer 0 pour sortir de l'application
}


int main( int argc, char **argv )
{
    // etape 1 : creer la fenetre
    Window window= create_window(1024, 640);
    if(window == NULL)
        return 1;       // erreur lors de la creation de la fenetre ou de l'init de sdl2

    // etape 2 : creer un contexte opengl pour pouvoir dessiner
    Context context= create_context(window);
    if(context == NULL)
        return 1;       // erreur lors de la creation du contexte opengl

    // etape 3 : creation des objets
    if(init() < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }

    // etape 4 : affichage de l'application, tant que la fenetre n'est pas fermee. ou que draw() ne renvoie pas 0
    run(window, draw);

    // etape 5 : nettoyage
    quit();
    release_context(context);
    release_window(window);
    return 0;
}
