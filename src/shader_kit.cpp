
// shader_kit light, bac a sable fragment shader, cf shader_toy 

#include <fstream>
#include <sstream>
#include <string>

#define GLEW_NO_GLU
#include "GL/glew.h"

#include "window.h"

#include "program.h"

#include "texture.h"
#include "mesh.h"
#include "wavefront.h"

#include "vec.h"
#include "mat.h"
#include "orbiter.h"

#include "text.h"
#include "widgets.h"


// program
const char *program_filename= "toy.glsl";
GLuint program;

// affichage des erreurs
std::string program_log;
bool program_failed;

GLuint vao;

//~ GLuint texture;
//~ Mesh cube;

Orbiter camera;
Widgets widgets;


// application
void reload_program( )
{
    if(program == 0)
        program= read_program(program_filename);
    else
        reload_program(program, program_filename);
    
    // recupere les erreurs, si necessaire
    program_format_errors(program, program_log);
    program_failed= (program_log.size() > 0);
}

//! compile les shaders et construit le programme + les buffers + le vertex array.
//! renvoie -1 en cas d'erreur.
int init( const int argc, const char **argv )
{
    widgets= create_widgets();
    
    camera= make_orbiter();
    
    program= 0;
    if(argc > 1) 
        program_filename= argv[1];
    reload_program();
    
    glGenVertexArrays(1, &vao);
    
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
    glDisable(GL_CULL_FACE);    // les faces mal orientees sont affichees avec des hachures oranges...
    
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    
    return 0;    
}

int quit( )
{
    release_widgets(widgets);
    // detruit les objets openGL
    //~ glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    return 0;
}

int draw( )
{
    // effacer l'image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(key_state('r'))
    {
        clear_key_state('r');
        reload_program();
    }
    
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
    
    Transform mvp= projection * view * model;
    Transform mvpInv= make_inverse(mvp);
    Transform mv= model * view;
    Transform mvInv= make_inverse(mv);
    
    // affiche l'objet
    if(program_failed == false)
    {
        glBindVertexArray(vao);
        glUseProgram(program);
        program_uniform(program, "mvpInvMatrix", mvpInv);
        program_uniform(program, "mvMatrix", mv);
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
    if(key_state('s'))
    {
        clear_key_state('s');
        screenshot("shader_kit.png");
    }
    
    // affiche les infos
    begin(widgets);
        begin_line(widgets);
            if(program_failed)
                label(widgets, "[error] program '%s'\n%s\n", program_filename, program_log.c_str());
            else
                label(widgets, "program %u, '%s' running...", program, program_filename);
        end_line(widgets);
    end(widgets);
    
    draw(widgets, window_width(), window_height());
    
    return 1;
}


int main( const int argc, const char **argv )
{
    Window w= create_window(1024, 640);
    if(w == NULL) 
        return 1;
    
    Context c= create_context(w);
    if(c == NULL) 
        return 1;
    
    // creation des objets opengl
    if(init(argc, argv) < 0)
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
