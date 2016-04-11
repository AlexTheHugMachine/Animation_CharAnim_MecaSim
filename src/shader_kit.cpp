
//! \file shader_kit.cpp shader_kit light, bac a sable fragment shader, cf shader_toy 

#include <cstdio>
#include <cstring>

#include "glcore.h"
#include "window.h"

#include "program.h"
#include "uniforms.h"

#include "texture.h"
#include "mesh.h"
#include "wavefront.h"

#include "vec.h"
#include "mat.h"
#include "orbiter.h"

#include "text.h"
#include "widgets.h"


// utilitaire
struct Filename
{
    char path[1024];
    
    Filename( ) { path[0]= 0; }
    Filename( const char *_filename ) { strcpy(path, _filename); }
    operator const char *( ) { return path; }
};

// program
Filename program_filename;
GLuint program;

// affichage des erreurs
std::string program_log;
int program_area;
bool program_failed;

Filename mesh_filename;
Mesh mesh;
unsigned int vertex_count;
GLuint vao;
bool wireframe= false;

std::vector<Filename> texture_filenames;
std::vector<GLuint> textures;

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
    program_area= program_format_errors(program, program_log);
    
    if(program_log.size() > 0)
        printf("[boom]\n%s\n", program_log.c_str());
    
    program_failed= (program_log.size() > 0);
}


// cherche un fichier avec l'extension ext dans les options
const char *option_find( std::vector<const char *>& options, const char *ext )
{
    for(unsigned int i= 0; i < (unsigned int) options.size() ; i++)
    {
        if(std::string(options[i]).rfind(ext) != std::string::npos)
        {
            const char *option= options[i];
            options[i]= options.back();
            options.pop_back();
            return option;
        }
    }
    
    return NULL;
}

//! compile les shaders et construit le programme + les buffers + le vertex array.
//! renvoie -1 en cas d'erreur.
int init( std::vector<const char *>& options )
{
    widgets= create_widgets();
    
    camera= make_orbiter();
    
    program= 0;
    const char *option;
    option= option_find(options, ".glsl");
    if(option != NULL)
    {
        program_filename= Filename(option);
        reload_program();
    }
    
    glGenVertexArrays(1, &vao);
    vertex_count= 3;
    
    option= option_find(options, ".obj");
    if(option != NULL)
    {
        mesh= read_mesh(option);
        if(mesh.positions.size() > 0)
        {
            mesh_filename= Filename(option);
            
            vao= make_mesh_vertex_format(mesh);
            vertex_count= (unsigned int) mesh.positions.size();
            
            Point pmin, pmax;
            mesh_bounds(mesh, pmin, pmax);
            orbiter_lookat(camera, center(pmin, pmax), distance(pmin, pmax));
        }
    }
    
    // charge les textures, si necessaire
    for(unsigned int i= 0; i < (unsigned int) options.size(); i++)
    {
        GLuint texture= read_texture(0, options[i]);
        if(texture > 0)
        {
            texture_filenames.push_back(Filename(options[i]));
            textures.push_back(texture);
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
    release_program(program);
    // detruit les objets openGL
    glDeleteVertexArrays(1, &vao);
    for(unsigned int i= 0; i < (unsigned int) textures.size(); i++)
        glDeleteTextures(1, &textures[i]);
    return 0;
}

int draw( void )
{
    
    if(wireframe)
    {
        glClearColor(1, 1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(2);
    }
    else
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
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
    Transform viewport= make_viewport(window_width(), window_height());
    
    Transform mvp= projection * view * model;
    Transform mvpInv= make_inverse(mvp);
    Transform mv= model * view;
    
    // affiche l'objet
    if(program_failed == false)
    {
        if(key_state('w'))
        {
            clear_key_state('w');
            wireframe= !wireframe;
        }
        
        // configuration minimale du pipeline
        glBindVertexArray(vao);
        glUseProgram(program);
        
        // affecte une valeur aux uniforms
        // transformations standards
        program_uniform(program, "modelMatrix", model);
        program_uniform(program, "modelInvMatrix", make_inverse(model));
        program_uniform(program, "viewMatrix", view);
        program_uniform(program, "viewInvMatrix", make_inverse(view));
        program_uniform(program, "projectionMatrix", projection);
        program_uniform(program, "projectionInvMatrix", make_inverse(projection));
        program_uniform(program, "viewportMatrix", viewport);
        program_uniform(program, "viewportInvMatrix", make_inverse(viewport));
        
        program_uniform(program, "mvpMatrix", mvp);
        program_uniform(program, "mvpInvMatrix", mvpInv);
        
        program_uniform(program, "mvMatrix", mv);
        program_uniform(program, "normalMatrix", make_normal_transform(mv));
        
        // interactions
        program_uniform(program, "viewport", make_vec2(window_width(), window_height()));
        program_uniform(program, "time", (float) SDL_GetTicks());
        program_uniform(program, "motion", make_vec3(mx, my, mb & SDL_BUTTON(1)));
        program_uniform(program, "mouse", make_vec3(mousex, mousey, mb & SDL_BUTTON(1)));
        
        // textures
        for(unsigned int i= 0; i < (unsigned int) textures.size(); i++)
        {
            char uniform[1024];
            sprintf(uniform, "texture%d", i);
            program_use_texture(program, uniform, i, textures[i]);
        }
        
        // go
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    }
    
    // affiche les infos
    begin(widgets);
    if(program_failed)
    {
        label(widgets, "[error] program '%s'", program_filename.path);
        begin_line(widgets);
        text_area(widgets, 20, program_log.c_str(), program_area);
    }
    else
    {
        label(widgets, "program '%s' running...", program_filename.path);
        if(mesh_filename[0] != 0)
        {
            begin_line(widgets);
            label(widgets, "mesh '%s', %u positions, %u texcoords, %u normals", mesh_filename.path, 
                (unsigned int) mesh.positions.size(),
                (unsigned int) mesh.texcoords.size(),
                (unsigned int) mesh.normals.size());
        }
        for(unsigned int i= 0; i < (unsigned int) texture_filenames.size(); i++)
        {
            begin_line(widgets);
            label(widgets, "texture%u '%s'", i, texture_filenames[i].path);
        }
    }
    end(widgets);
    
    draw(widgets, window_width(), window_height());
    
    
    if(key_state('s'))
    {
        clear_key_state('s');
        screenshot("shader_kit.png");
    }
    
    static bool video= false;
    if(key_state(SDLK_RETURN))
    {
        clear_key_state(SDLK_RETURN);
        video= !video;
        
        if(video) 
            printf("start video capture...\n");
        else 
            printf("stop video capture.\n");
    }
    if(video) 
        capture("shader_kit");
    
    if(key_state('c'))
    {
        clear_key_state('c');
        write_orbiter(camera, "orbiter.txt");
    }
    if(key_state('v'))
    {
        clear_key_state('v');
        camera= read_orbiter("orbiter.txt");
    }
    
    return 1;
}


int main( int argc, char **argv )
{
    if(argc == 1)
    {
        printf("usage: %s shader.glsl [mesh.obj] [texture0.png [texture1.png]]\n", argv[0]);
        return 0;
    }
    
    Window window= create_window(1024, 640);
    if(window == NULL) 
        return 1;
    
    Context context= create_context(window);
    if(context == NULL) 
        return 1;
    
    // creation des objets opengl
    std::vector<const char *> options(argv + 1, argv + argc);
    if(init(options) < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }
    
    // affichage de l'application
    run(window, draw);

    // nettoyage
    quit();
    release_context(context);
    release_window(window);
    return 0;
}
