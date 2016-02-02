
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


GLuint texture;

Orbiter camera;
Mesh cube;

Text console;

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
    //~ cube= read_mesh("data/triangle_bigguy.obj");
#else
    // construit un mesh
    
    cube= make_mesh(GL_TRIANGLE_STRIP);

    vertex_texcoord(cube, 0, 0);
    push_vertex(cube, make_vec3(0, 0, 0));

    //~ vertex_texcoord(cube, 0, 1);
    //~ push_vertex(cube, make_vec3(0, 1, 0));

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
    console= create_text();
    
#if 1
    vec3 pmin, pmax;
    bounds(cube, pmin, pmax);
    camera= make_orbiter_lookat( (pmin + pmax) / 2, distance(pmin, pmax) );
#else    
    camera= make_orbiter_lookat( make_vec3(0.5, 0.5, 0.5), 5 );
#endif
    
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
    //~ glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    return 0;
}

int quit( )
{
    release_mesh(cube);
    release_text(console);
    // detruit les objets openGL
    glDeleteTextures(1, &texture);
    return 0;
}

int draw( )
{
    // effacer l'image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    int x, y;
    unsigned int button= SDL_GetRelativeMouseState(&x, &y);
    
    if(button & SDL_BUTTON(1))
        orbiter_rotation(camera, x, y);      // orbit
    else if(button & SDL_BUTTON(2))
        orbiter_translation(camera, (float) x / (float) window_width(), (float) y / (float) window_height()); // pan
    else if(button & SDL_BUTTON(3))
        orbiter_move(camera, x);           // dolly
    
    if(key_state(' '))
    {
        clear_key_state(' ');
        printf("boom\n");
    }
    
    
    // initialiser les transformations
    mat4 model= make_identity();
    mat4 view= orbiter_view_matrix(camera);
    mat4 projection= orbiter_projection_matrix(camera, window_width(), window_height(), 45);
    
    draw(cube, model, view, projection, texture);

    text_clear(console);
    text_print(console, 0, 1, "print");
    //~ text_print(console, 0, 2, "\2\2\2\2\2\2\2\2");
    draw_text(console, window_width(), window_height());
    
    return 1;
}


int main( int argc, char **argv )
{
    Window w= create_window(1024, 768);
    if(w == NULL) 
        return 1;
    
    Context c= create_context(w);        // cree un contexte opengl 3.3, par defaut
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
