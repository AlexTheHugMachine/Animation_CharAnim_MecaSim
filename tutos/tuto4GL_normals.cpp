
/*! \file tuto4GL_normals.cpp
    affichage d'un maillage + vertex array object + vertex buffer + shader program + uniforms
    configure un 2ieme attribut, les normales
 */

#include "window.h"
#include "vec.h"
#include "mat.h"

#include "program.h"
#include "uniforms.h"

#include "orbiter.h"
#include "mesh.h"
#include "wavefront.h"

#include <stdio.h>

// shader program
GLuint program;

// vertex array object
GLuint vao;
GLuint vertex_buffer;
GLuint normal_buffer;
unsigned int vertex_count;

// camera
Orbiter camera;


int init( )
{
    // compile un shader program
    program= read_program("tutos/tuto4GL_normals.glsl");
    program_print_errors(program);

    // charge un objet
    Mesh mesh= read_mesh("data/bigguy.obj");
    vertex_count= (unsigned int) mesh.positions.size();

    // camera
    Point pmin, pmax;
    mesh_bounds(mesh, pmin, pmax);

    camera= make_orbiter_lookat(center(pmin, pmax), distance(pmin, pmax));

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    // dimensionne le buffer actif sur array_buffer, l'alloue et l'initialise avec les positions des sommets de l'objet
    glBufferData(GL_ARRAY_BUFFER,
        /* length */ sizeof(vec3) * mesh.positions.size(),
        /* data */ &mesh.positions.front(),
        /* usage */ GL_STATIC_DRAW);
    // GL_STATIC_DRAW decrit l'utilisation du contenu du buffer. dans ce cas, utilisation par draw, sans modifications

    // on recommence avec les normales
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mesh.normals.size(), &mesh.normals.front(), GL_STATIC_DRAW);

/*  attention: c'est normal_buffer qui est selectionne maintenant sur array_buffer, plus vertex_buffer...
 */

    // creation d'un vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // recuperer l'identifiant de l'attribut : cf in vec3 position; dans le vertex shader
    GLint attribute= glGetAttribLocation(program, "position");
    if(attribute < 0)
        return -1;

    // re-selectionne vertex buffer pour configurer les positions
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    // format et organisation des donnees dans le vertex buffer selectionne sur array_buffer,
    glVertexAttribPointer(attribute, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, /* offset */ 0);
    glEnableVertexAttribArray(attribute);

    // on recommence pour les normales
    attribute= glGetAttribLocation(program, "normal");
    if(attribute < 0)
        return -1;

    // re-selectionne normal_buffer pour configurer les normales
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glVertexAttribPointer(attribute, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, /* offset */ 0);  // in vec3 normal;
    glEnableVertexAttribArray(attribute);

/*  autre solution pour le meme resultat :

    #include "buffer.h"

    GLuint vao= make_vertex_format();

    vertex_buffer= make_vertex_buffer(vao, glGetAttribLocation(program, "position"), 3, GL_FLOAT);
    normal_buffer= make_vertex_buffer(vao, glGetAttribLocation(program, "normal"), 3, GL_FLOAT);
 */


/*  creer 2 buffers pour ranger les positions et les normales est arbitraire, on peut tres bien ne creer qu'un seul buffer,
    par contre, la description du format / de l'organisation sera differente...
    ce sont les parametres stride et offset de glVertexAttribPointer() qui permettent d'organiser les donnees librement,
    ils permettent d'iterer sur les donnees, offset definit le premier element et stride permet de passer au suivant.

    exemple 1: un seul buffer, toutes les positions, puis toutes les normales

    PPPPPPPPNNNNNNNN

    offset(position)= 0;
    stride(position)= 0; // par definition stride(position) == sizeof(vec3)

    offset(normal)= sizeof(vec3) * size(positions);
    stride(normal)= 0; // ou sizeof(vec3)

    exemple 2: un seul buffer, alterner les donnees, position + normale sommet 0, position + normale sommet 1, etc.

    PNPNPNPNPNPNPN

    offset(position)= 0;
    stride(position)= sizeof(vec3) + sizeof(vec3);  // la prochaine position se trouve apres la normale du sommet

    offset(normal)= sizeof(vec3);   // la premiere normale se trouve apres la premiere position
    stride(normal)= sizeof(vec3) + sizeof(vec3);

    exemple 3: et pour normale suivie de position ?

    NPNPNPNPNPNPNPNP

 */

    // nettoyage
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // etat par defaut openGL
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClearDepthf(1);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    // glViewport(0, 0, window_width(), window_height()) // deja fait dans run( )
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
        orbiter_rotation(camera, mx, my);

    else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
        // approche / eloigne l'objet
        orbiter_move(camera, mx);

    else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
        // deplace le point de rotation
        orbiter_translation(camera, (float) mx / (float) window_width(), (float) my / (float) window_height());


/*  configuration minimale du pipeline
    vertex array
    shader program
    uniforms du shader program
 */
    glBindVertexArray(vao);

    glUseProgram(program);

    // recupere le point de vue et la projection de la camera
    Transform model= make_identity();
    Transform view= orbiter_view_transform(camera);
    Transform projection= orbiter_projection_transform(camera, window_width(), window_height(), 45);

    // compose les matrices pour passer du repere local de l'objet au repere projectif
    Transform mvp= projection * view * model;
    Transform mv= view * model;

    GLuint location;
    location= glGetUniformLocation(program, "mvpMatrix");
    glUniformMatrix4fv(location, 1, GL_TRUE, &mvp.m[0][0]);

    // les normales ne subissent pas tout a fait la meme transformation que les sommets
    program_uniform(program, "mvMatrix", make_normal_transform(mv));

    program_uniform(program, "color", make_vec3(1, 0.5, 0.));

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
