
//! \file shader_kit.cpp shader_kit light, bac a sable fragment shader, cf shader_toy 

#include <cstdio>
#include <cstring>

#ifndef _MSC_VER
    #include <sys/stat.h>
#else
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

#include <chrono>

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
// renvoie la date de la derniere modification d'un fichier
long int timestamp( const char *filename )
{
#ifndef _MSC_VER
    struct stat info;
    if(stat(filename, &info) < 0)
        return 0;

    // verifie aussi que c'est bien un fichier standard
    if(S_ISREG(info.st_mode))
        return info.st_mtime;

#else
    struct _stat64 info;
    if(_stat64(filename, &info) < 0)
        return 0;

    // verifie aussi que c'est bien un fichier standard
    if(info.st_mode & _S_IFREG)
        return info.st_mtime;
#endif

    return 0;
}




// program
const char *program_filename;
GLuint program;

// affichage des erreurs
std::string program_log;
int program_area;
bool program_failed;

const char *mesh_filename;
Mesh mesh;
Point mesh_pmin;
Point mesh_pmax;
int vertex_count;
GLuint vao;
bool wireframe= false;

std::vector<const char *> texture_filenames;
std::vector<GLuint> textures;

Orbiter camera;
Widgets widgets;

// application
long int last_load= 0;
void reload_program( )
{
    if(program == 0)
        program= read_program(program_filename);
    else
        reload_program(program, program_filename);
    
    // conserve la date du fichier
    last_load= timestamp(program_filename);
    
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
        if(options[i] != nullptr && std::string(options[i]).rfind(ext) != std::string::npos)
        {
            const char *option= options[i];
            options[i]= nullptr;
            return option;
        }
    }
    
    return nullptr;
}

//! compile les shaders et construit le programme + les buffers + le vertex array.
//! renvoie -1 en cas d'erreur.
int init( std::vector<const char *>& options )
{
    widgets= create_widgets();
    camera= Orbiter();
    
    program= 0;
    const char *option;
    option= option_find(options, ".glsl");
    if(option != nullptr)
    {
        program_filename= option;
        reload_program();
    }
    
    vao= 0;
    mesh_pmin= Point(normalize(Vector(-1, -1, 0)) * 2.5f);
    mesh_pmax= Point(normalize(Vector( 1,  1, 0)) * 2.5f);
    
    option= option_find(options, ".obj");
    if(option != nullptr)
    {
        mesh= read_mesh(option);
        if(mesh.vertex_count() > 0)
        {
            mesh_filename= option;
            
            vao= mesh.create_buffers(mesh.has_texcoord(), mesh.has_normal(), mesh.has_color(), mesh.has_material_index());
            vertex_count= mesh.vertex_count();
            
            mesh.bounds(mesh_pmin, mesh_pmax);
            camera.lookat(mesh_pmin, mesh_pmax);
        }
        
        // ou generer une erreur ? 
    }
    
    if(vao == 0)
    {
        glGenVertexArrays(1, &vao);
        vertex_count= 3;
    }
    
    // charge les textures, si necessaire
    for(int i= 0; i < int(options.size()); i++)
    {
        if(options[i] == nullptr)
            continue;
        
        GLuint texture= read_texture(0, options[i]);
        if(texture > 0)
        {
            texture_filenames.push_back(options[i]);
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
    // detruit les objets openGL
    release_widgets(widgets);
    release_program(program);
    mesh.release();
    
    glDeleteTextures(textures.size(), textures.data());
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
    
    // verification de la date du fichier source des shaders...
    static float last_time= 0;
    // toutes les secondes, ca suffit, pas tres malin de le faire 60 fois par seconde...
    if(global_time() > last_time + 400)
    {
        if(timestamp(program_filename) != last_load)
            // date modifiee, recharger les sources et recompiler...
            reload_program();
        
        // attends le chargement et la compilation des shaders... au cas ou ce soit plus long qu'une seconde...
        // (oui ca arrive...)
        last_time= global_time();
    }
    
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
        camera.rotation(mx, my);      // tourne autour de l'objet
    else if(mb & SDL_BUTTON(3))
        camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height()); // deplace le point de rotation
    else if(mb & SDL_BUTTON(2))
        camera.move(mx);           // approche / eloigne l'objet
    
    SDL_MouseWheelEvent wheel= wheel_event();
    if(wheel.y != 0)
    {
        clear_wheel_event();
        camera.move(8.f * wheel.y);  // approche / eloigne l'objet
    }
    
    // recupere les transformations
    Transform model= Identity();
    Transform view= camera.view();
    Transform projection= camera.projection(window_width(), window_height(), 45);
    Transform viewport= Viewport(window_width(), window_height());
    
    Transform mvp= projection * view * model;
    Transform mvpInv= Inverse(mvp);
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
        program_uniform(program, "modelInvMatrix", model.inverse());
        program_uniform(program, "viewMatrix", view);
        program_uniform(program, "viewInvMatrix", view.inverse());
        program_uniform(program, "projectionMatrix", projection);
        program_uniform(program, "projectionInvMatrix", projection.inverse());
        program_uniform(program, "viewportMatrix", viewport);
        program_uniform(program, "viewportInvMatrix", viewport.inverse());
        
        program_uniform(program, "mvpMatrix", mvp);
        program_uniform(program, "mvpInvMatrix", mvpInv);
        
        program_uniform(program, "mvMatrix", mv);
        program_uniform(program, "mvInvMatrix", mv.inverse());
        program_uniform(program, "normalMatrix", mv.normal());
        
        // interactions
        program_uniform(program, "viewport", vec2(window_width(), window_height()));
        program_uniform(program, "time", (float) global_time());
        program_uniform(program, "motion", vec3(mx, my, mb & SDL_BUTTON(1)));
        program_uniform(program, "mouse", vec3(mousex, mousey, mb & SDL_BUTTON(1)));
        
        // textures
        for(int i= 0; i < int(textures.size()); i++)
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
        label(widgets, "[error] program '%s'", program_filename);
        begin_line(widgets);
        text_area(widgets, 20, program_log.c_str(), program_area);
    }
    else
    {
        label(widgets, "program '%s' running...", program_filename);
        if(mesh_filename && mesh_filename[0])
        {
            begin_line(widgets);
            label(widgets, "mesh '%s', %d vertices %s %s", mesh_filename, mesh.vertex_count(),
                mesh.texcoord_buffer_size() ? "texcoords" : "", mesh.normal_buffer_size() ? "normals" : "");
        }
        for(unsigned int i= 0; i < (unsigned int) texture_filenames.size(); i++)
        {
            begin_line(widgets);
            label(widgets, "texture%u '%s'", i, texture_filenames[i]);
        }
    }
    
    end(widgets);
    
    draw(widgets, window_width(), window_height());
    
    
    if(key_state('s'))
    {
        clear_key_state('s');
        
        static int calls= 1;
        printf("screenshot %d...\n", calls);
        screenshot("shader_kit", calls++);
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
        camera.write_orbiter("orbiter.txt");
    }
    if(key_state('v'))
    {
        clear_key_state('v');
        if(camera.read_orbiter("orbiter.txt") < 0)
        {
            camera= Orbiter();
            camera.lookat(mesh_pmin, mesh_pmax);
        }
    }
    if(key_state('f'))
    {
        clear_key_state('f');
        camera= Orbiter();
        camera.lookat(mesh_pmin, mesh_pmax);
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
    if(window == nullptr) 
        return 1;
    
    Context context= create_context(window);
    if(context == nullptr) 
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
