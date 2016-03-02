
//! \file shader_kit.cpp shader_kit light, bac a sable fragment shader, cf shader_toy 

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
char program_filename[1024]= { 0 };
GLuint program;

// affichage des erreurs
std::string program_log;
int program_area;
bool program_failed;


//~ GLuint texture;
char mesh_filename[1024]= { 0 };
Mesh mesh;
GLuint vao;
unsigned int vertex_count;

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
    if(program_log.size() > 0)
        printf("[boom]\n%s\n", program_log.c_str());
    
    program_failed= (program_log.size() > 0);
    program_area= 1;
}


// cherche un fichier avec l'extension ext dans les options
const char *option_find( const int argc, const char **argv, const char *ext )
{
    for(int i= 1; i < argc; i++)
    {
        std::string option(argv[i]);
        if(option.rfind(ext) != std::string::npos)
            return argv[i];
    }
    
    return NULL;
}

//! compile les shaders et construit le programme + les buffers + le vertex array.
//! renvoie -1 en cas d'erreur.
int init( const int argc, const char **argv )
{
    widgets= create_widgets();
    
    camera= make_orbiter();
    
    program= 0;
    const char *option;
    option= option_find(argc, argv, ".glsl");
    if(option != NULL)
    {
        strcpy(program_filename, option);
        reload_program();
    }
    
    glGenVertexArrays(1, &vao);
    vertex_count= 3;
    
    option= option_find(argc, argv, ".obj");
    if(option != NULL)
    {
        mesh= read_mesh(option);
        if(mesh.positions.size() > 0)
        {
            strcpy(mesh_filename, option);
            
            vao= make_mesh_vertex_format(mesh);
            vertex_count= (unsigned int) mesh.positions.size();
            
            Point pmin, pmax;
            mesh_bounds(mesh, pmin, pmax);
            orbiter_lookat(camera, center(pmin, pmax), distance(pmin, pmax));
        }
    }
    
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
    release_program(program);
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
    int mousex, mousey;
    SDL_GetMouseState(&mousex, &mousey);
    
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
    
    // affiche l'objet
    if(program_failed == false)
    {
        glBindVertexArray(vao);
        
        glUseProgram(program);
        program_uniform(program, "modelMatrix", model);
        program_uniform(program, "modelInvMatrix", make_inverse(model));
        program_uniform(program, "viewMatrix", view);
        program_uniform(program, "viewInvMatrix", make_inverse(view));
        program_uniform(program, "projectionMatrix", projection);
        program_uniform(program, "projectionInvMatrix", make_inverse(projection));
        
        program_uniform(program, "mvpMatrix", mvp);
        program_uniform(program, "mvpInvMatrix", mvpInv);
        
        program_uniform(program, "mvMatrix", mv);
        program_uniform(program, "normalMatrix", make_normal_transform(mv));
        
        program_uniform(program, "viewport", make_vec2(window_width(), window_height()));
        program_uniform(program, "time", (float) SDL_GetTicks());
        program_uniform(program, "motion", make_vec3(mx, my, mb & SDL_BUTTON(1)));
        program_uniform(program, "mouse", make_vec3(mousex, mousey, mb & SDL_BUTTON(1)));
        
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    }
    
    if(key_state('s'))
    {
        clear_key_state('s');
        screenshot("shader_kit.png");
    }
    
    // affiche les infos
    begin(widgets);
    if(program_failed)
    {
        label(widgets, "[error] program '%s'", program_filename);
        begin_line(widgets);
        text_area(widgets, 20, program_log.c_str(), program_area);
    }
    else
    {
        label(widgets, "program '%s' running...", program_filename);
        if(mesh_filename[0] != 0)
        {
            begin_line(widgets);
            label(widgets, "mesh '%s', %u positions, %u texcoords, %u normals", mesh_filename, 
                (unsigned int) mesh.positions.size(),
                (unsigned int) mesh.texcoords.size(),
                (unsigned int) mesh.normals.size());
        }
    }
    end(widgets);
    
    draw(widgets, window_width(), window_height());
    
    return 1;
}


int main( const int argc, const char **argv )
{
    if(argc == 1)
    {
        printf("usage: %s shader.glsl [mesh.obj] [texture0.png [texture1.png]]\n", argv[0]);
        return 0;
    }
    
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
