
//! \file tuto4GL.cpp

#include "window.h"
#include "vec.h"
#include "mat.h"

#include "program.h"

#include "orbiter.h"
#include "mesh.h"
#include "wavefront.h"

#include <stdio.h>

// shader program
GLuint program;

// vertex array object
GLuint vao;
GLuint vertex_buffer;
int vertex_count;

// camera
Orbiter camera;


int init( )
{
    // compile un shader program
    program= read_program("tutos/tuto4GL.glsl");
    program_print_errors(program);

    // charge un objet
    Mesh mesh= read_mesh("data/bigguy.obj");
    vertex_count= mesh.vertex_count();

    // camera
    Point pmin, pmax;
    mesh.bounds(pmin, pmax);
    camera.lookat(pmin, pmax);

/*  creation du vertex buffer :
        en plusieurs etapes : creer le buffer, allouer un espace de stockage et transferer des donnees

    rappel: l'objet manipule par les fonctions de l'api est implicite, il doit etre selectionne avant.
    pour un vertex buffer, utiliser GL_ARRAY_BUFFER
 */
    glGenBuffers(1, &vertex_buffer);
    // selectionne le buffer pour l'initialiser
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    // dimensionne le buffer actif sur array_buffer, l'alloue et l'initialise avec les positions des sommets de l'objet
    glBufferData(GL_ARRAY_BUFFER,
        /* length */    mesh.vertex_buffer_size(),
        /* data */      mesh.vertex_buffer(),
        /* usage */     GL_STATIC_DRAW);
    // GL_STATIC_DRAW decrit l'utilisation du contenu du buffer. dans ce cas, utilisation par draw, sans modifications

/*  le buffer est cree et initialise, mais il est toujours selectionne sur array_buffer,
    on peut le de-selectionner (ou pas) pour revenir a l'etat par defaut :
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    le buffer 0 est un objet null
 */

/*  description du format des sommets :
        creer un vertex array object, c'est l'objet qui represente le format des sommets
        recuperer l'identifiant d'un attribut declare par le vertex shader,
        decrire l'organisation des donnees a l'interieur du vertex buffer selectionne sur array_buffer
        activer l'attribut
 */
    // creation d'un vertex array object
    glGenVertexArrays(1, &vao);
    // pour configurer le vao, il faut le selectionner
    glBindVertexArray(vao);

    // recuperer l'identifiant de l'attribut : cf in vec3 position; dans le vertex shader
    GLint attribute= glGetAttribLocation(program, "position");
    if(attribute < 0)
        // gros probleme...
        return -1;
    // le shader program doit etre compile sans erreurs avant...

    // format et organisation des donnees dans le vertex buffer selectionne sur array_buffer,
    glVertexAttribPointer(attribute, 3, GL_FLOAT,       // l'attribut est un vec3,
        /* not normalized */    GL_FALSE,               // les valeurs ne sont pas normalisees entre 0, et 1
        /* stride */            0,                      // les vec3 sont ranges les uns a la suite des autres
        /* offset */            0);                     // et se trouvent au debut du buffer

    glEnableVertexAttribArray(attribute);
    // cette description est stockee dans le vertex array object selectionne...

/*  le vertex buffer utilise est celui actuellement selectionne sur array_buffer.
    eventuellement, il faut selectionner le vertex buffer, avant glVertexAttribPointer()
 */

    // nettoyage
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // etat par defaut openGL
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClearDepthf(1);
    // glViewport(0, 0, window_width(), window_height()) // deja fait dans run( )

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    return 0;
}

int quit( )
{
    release_program(program);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteVertexArrays(1, &vao);
    return 0;
}

int draw( )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // todo recuperez les mouvements de la souris pour deplacer la camera, cf tutos/tuto6.cpp
    // recupere les mouvements de la souris, utilise directement SDL2
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);

    // deplace la camera
    if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
        // tourne autour de l'objet
        camera.rotation(mx, my);

    else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
        // approche / eloigne l'objet
        camera.move(mx);

    else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
        // deplace le point de rotation
        camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());


/*  configuration minimale du pipeline
    vertex array
    shader program
    uniforms du shader program
 */
    glBindVertexArray(vao);

    glUseProgram(program);

    // recupere le point de vue et la projection de la camera
    Transform model= Identity();
    Transform view= camera.view();
    Transform projection= camera.projection(window_width(), window_height(), 45);

    // compose les matrices pour passer du repere local de l'objet au repere projectif
    Transform mvp= projection * view * model;

    GLuint location;
    location= glGetUniformLocation(program, "mvpMatrix");
    glUniformMatrix4fv(location, 1, GL_TRUE, mvp.buffer());

/*  plus direct :
    #include "uniforms.h"
    program_uniform(program, "mvpMatrix", mvp);
 */

    location= glGetUniformLocation(program, "color");
    glUniform3f(location, 1, 0.5, 0);
//  ou program_uniform(program, "color", 1, 0.5, 0);

    glDrawArrays(GL_TRIANGLES, 0, vertex_count);

    // nettoyage
    glUseProgram(0);
    glBindVertexArray(0);
    return 1;
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
