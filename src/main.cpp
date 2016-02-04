
//! tutorial : exemple minimaliste opengl 3 core profile

#define GLEW_NO_GLU
#include "GL/glew.h"

#include "window.h"

#include "texture.h"
#include "mesh.h"
#include "wavefront.h"

#include "vec.h"
#include "mat.h"
#include "orbiter.h"

#include "text.h"
#include "widgets.h"


GLuint texture;

Orbiter camera;
Mesh cube;

Text console;
Widgets widgets;

//! compile les shaders et construit le programme + les buffers + le vertex array.
//! renvoie -1 en cas d'erreur.
int init( )
{
    // charger une texture
    texture= read_texture(0, "data/debug2x2red.png");
    if(texture == 0) 
        return -1;
    
#if 1
    // charge un fichier obj
    cube= read_mesh("data/bigguy.obj");

#else
    // construit un mesh
    cube= make_mesh(GL_TRIANGLE_STRIP);

    vertex_texcoord(cube, 0, 0);
    push_vertex(cube, make_vec3(0, 0, 0));

    vertex_texcoord(cube, 1, 0);
    push_vertex(cube, make_vec3(1, 0, 0));
    
    vertex_texcoord(cube, 0, 1);
    push_vertex(cube, make_vec3(0, 1, 0));

    vertex_texcoord(cube, 1, 1);
    push_vertex(cube, make_vec3(1, 1, 0));

    restart_strip(cube);
    
    vertex_texcoord(cube, 0, 0);
    push_vertex(cube, make_vec3(0, 0, 1));

    vertex_texcoord(cube, 1, 0);
    push_vertex(cube, make_vec3(1, 0, 1));
    
    vertex_texcoord(cube, 0, 1);
    push_vertex(cube, make_vec3(0, 1, 1));

    vertex_texcoord(cube, 1, 1);
    push_vertex(cube, make_vec3(1, 1, 1));
#endif
    
    // 
    Point pmin, pmax;
    bounds(cube, pmin, pmax);
    camera= make_orbiter_lookat( pmin + make_vector(pmin, pmax) / 2, distance(pmin, pmax) );
    
    //
    console= create_text();
    widgets= create_widgets();
    
    // nettoyage
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // etat openGL par defaut
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glClearDepth(1.f);
    
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    //~ glEnable(GL_CULL_FACE); // n'affiche que les faces correctement orientees...
    glDisable(GL_CULL_FACE);    // les faces mal orientiees sont affichees avec des hachures oranges...
    
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    
    return 0;
}

int quit( )
{
    release_mesh(cube);
    release_text(console);
    release_widgets(widgets);
    // detruit les objets openGL
    glDeleteTextures(1, &texture);
    return 0;
}

int draw( )
{
    // effacer l'image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // recupere les mouvements de la souris
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
    
    // deplace la camera
    if(mb & SDL_BUTTON(1))
        orbiter_rotation(camera, mx, my);      // tourne autour de l'objet
    else if(mb & SDL_BUTTON(2))
        orbiter_translation(camera, (float) mx / (float) window_width(), (float) my / (float) window_height()); // deplace le point de rotation
    else if(mb & SDL_BUTTON(3))
        orbiter_move(camera, mx);           // approche / eloigne l'objet
    
    // recupere les transformations
    Transform model= make_identity();
    Transform view= orbiter_view_transform(camera);
    Transform projection= orbiter_projection_transform(camera, window_width(), window_height(), 45);
    
    // affiche l'objet
    draw(cube, model, view, projection, texture);

#if 0
    clear(console);
    print(console, 0, 1, "print");
    printf(console, 0, 2, "printf %d %d", mx, my);
    
    if(key_state(' '))
        printf_background(console, 0, 3, "  boom  ");
    
    draw(console, window_width(), window_height());

#else    
    static int button1= 0;
    static char edit1[32]= { 0 };
    
    begin(widgets);
        begin_line(widgets);
            label(widgets, "line1");
            label(widgets, "label1");
            label(widgets, "label2");
        end_line(widgets);
        
        begin_line(widgets);
            label(widgets, "line2");
            label(widgets, "label1");
            label(widgets, "label2");
            if(edit(widgets, sizeof(edit1), edit1))
            {
                printf("edit: '%s'\n", edit1);
            }
        end_line(widgets);
        
        begin_line(widgets);
            label(widgets, "line3");
            labelf(widgets, "format %-3d %-3d", mx, my);        // printf formate
            if(button(widgets, "button1", button1))
            {
                printf("button: %d\n", button1);
            }
        end_line(widgets);
    end(widgets);
    
    draw(widgets, window_width(), window_height());
#endif
    
    return 1;
}


int main( int argc, char **argv )
{
    Window w= create_window(1024, 640);
    if(w == NULL) 
        return 1;
    
    Context c= create_context(w);
    if(c == NULL) 
        return 1;
    
    // creation des objets opengl
    if(init() < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }
    
    // affichage de l'application
    run(w);

    // nettoyage
    quit();
    release_context(c);
    release_window(w);
    return 0;
}
