
//! \file tuto3GL.cpp passer des parametres a un shader program, afficher quelques triangles et les deplacer...

#include "window.h"
#include "program.h"
#include "mesh.h"
#include "wavefront.h"
#include <stdio.h>


// identifiant du shader program
GLuint program;

// identifiant du vertex array object
GLuint vao;


int init( )
{
    // charge 12 triangles, soit un cube...
    Mesh cube= read_mesh("data/flat_bbox.obj");
    printf("  %d positions\n", cube.vertex_count());

    // compile le shader program, le program est selectionne
    program= read_program("tutos/tuto3GL.glsl");
    
    /* affiche les erreurs de compilation des shaders
    program_print_errors(program);
    */

    // transfere les 36 positions dans le tableau declare par le vertex shader
    // etape 1 : recuperer l'identifiant de l'uniform
    GLint location= glGetUniformLocation(program, "positions"); // uniform vec3 positions[36];
    // etape 2 : modifier sa valeur
    glUniform3fv(location, cube.vertex_count(), cube.vertex_buffer());

    // mesh n'est plus necessaire
    cube.release();

    // creer un vertex array object
    glGenVertexArrays(1, &vao);

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
    glUniform4f(location, 1, 0.5, 0, 1);                // cf fragment shader vec4 color;

/*
    modifiez la valeur de time, declare dans le vertex shader pour modifier la position du cube dans le repere du monde,
    en fonction du temps. vous pouvez utiliser la fonction mod ou modf pour creer un mouvement cyclique

    utilisez la fonction SDL_GetTicks( ), pour connaitre l'heure exprimee en milli secondes

    float time= SDL_GetTicks();
    location= glGetUniformLocation(program, "time");
    glUniform1f(location, time);
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
