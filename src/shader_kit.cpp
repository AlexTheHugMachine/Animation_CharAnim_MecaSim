
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

const char *program_filename= "toy.glsl";
GLuint program;
bool program_failed;

GLuint vao;

//~ GLuint texture;
//~ Mesh cube;

Orbiter camera;
Widgets widgets;

// utilitaires shader program, modifications de program.cpp, exporte les erreurs
std::string program_log;

namespace tools {

std::string read( const char *filename )
{
    std::stringbuf source;
    std::ifstream in(filename);
    if(in.good() == false)
        program_log.append("[error] loading shader '").append(filename).append("'...\n");
    else
        program_log.append("loading shader '").append(filename).append("'...\n");
    
    in.get(source, 0);        // lire tout le fichier, le caractere '\0' ne peut pas se trouver dans le source de shader
    return source.str();
}


void print_line( const std::string& source, int line_id )
{
    int line= 1;
    for(unsigned int i= 0; i < source.size(); i++)
    {
        if(line > line_id)
            break;
        if(line == line_id)
            program_log.push_back(source[i]);
        if(source[i] == '\n')
            line++;
    }
}
    
void print_errors( const char *log, const std::string& source )
{
    // affiche la ligne du source + l'erreur associee
    int last_string= -1;
    int last_line= -1;
    
    for(int i= 0; log[i] != 0; i++)
    {
        int string_id= 0, line_id= 0, position= 0;
        if(sscanf(&log[i], "%d ( %d ) : %n", &string_id, &line_id, &position) == 2        // nvidia syntax
        || sscanf(&log[i], "%d : %d (%*d) : %n", &string_id, &line_id, &position) == 2  // mesa syntax
        || sscanf(&log[i], "ERROR : %d : %d : %n", &string_id, &line_id, &position) == 2  // ati syntax
        || sscanf(&log[i], "WARNING : %d : %d : %n", &string_id, &line_id, &position) == 2)  // ati syntax
        {
            if(string_id != last_string || line_id != last_line)
            {
                program_log.append("\n");
                print_line(source, line_id);
                program_log.append("\n");
            }
        }
        
        // affiche l'erreur
        for(i+= position; log[i] != 0; i++)
        //~ for(; log[i] != 0; i++)
        {
            program_log.push_back(log[i]);
            if(log[i] == '\n') break;
        }
        
        last_string= string_id;
        last_line= line_id;
    }
}

//! affiche les erreurs de compilation de shader.
bool shader_errors( const GLuint shader, const std::string& source )
{
    if(shader == 0) 
        return true;
    
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_TRUE) 
        return false;
    
    char errors[4096];
    glGetShaderInfoLog(shader, sizeof(errors) -1, NULL, errors);
    program_log.append("[shader errors]\n");
    print_errors(errors, source);
    return true;
}

//! affiche les erreurs de link du programme.
bool program_errors( const GLuint program )
{
    if(program == 0) 
        return true;
    
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_TRUE) 
        return false;
    
    char errors[4096];
    glGetProgramInfoLog(program, sizeof(errors) -1, NULL, errors);
    program_log.append("[program errors]\n").append(errors).append("\n");
    return true;
}

std::string prepare_source( std::string file, const std::string& definitions )
{
    if(file.empty()) 
        return std::string();
    
    // un peu de gymnastique, #version doit rester sur la premiere ligne, meme si on insere des #define dans le source
    std::string source;
    
    // recupere la ligne #version
    std::string version;
    unsigned long int b= file.find("#version");
    if(b != std::string::npos)
    {
        unsigned long int e= file.find('\n', b);
        if(e != std::string::npos)
        {
            version= file.substr(0, e +1);
            file.erase(0, e +1);
            
            if(file.find("#version") != std::string::npos)
            {
                program_log.append("[error] found several #version directives. failed.\n");
                return std::string();
            }
        }
    }
    else
    {
        program_log.append("[error] no #version directive found. failed.\n");
        return std::string();
    }
    
    // reconstruit le source complet
    if(definitions.empty() == false)
    {
        source.append(version);                         // insere la version
        source.append(definitions).append("\n");        // insere les definitions
        source.append(file);                            // insere le source
    }
    else
    {
        source.append(version);                         // re-insere la version (supprimee de file)
        source.assign(file);                            // insere le source
    }
    
    return source;
}

GLuint compile_shader( const GLenum shader_type, const std::string& source )
{
    if(source.empty()) 
        return 0;
    
    GLuint shader= glCreateShader(shader_type);
    if(shader == 0) 
        return 0;
    
    const char *sources= source.c_str();
    glShaderSource(shader, 1, &sources, NULL);
    glCompileShader(shader);
    if(shader_errors(shader, source))
    {
        glDeleteShader(shader);
        shader= 0;
    }
    
    return shader;
}

int reload_program_sources( GLuint program, const std::string& vertex_source, const std::string& fragment_source )
{
    if(program == 0) 
        return -1;

    // creer et compiler un vertex shader et un fragment shader
    GLuint vertex_shader= compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader= compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    if(vertex_shader == 0 || fragment_shader == 0)
    {
        if(vertex_shader > 0)
            glDeleteShader(vertex_shader);
        if(fragment_shader > 0)
            glDeleteShader(fragment_shader);
        return -1;
    }
    
    // associer les shaders au program
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    
    // linker les shaders pour obtenir un program utilisable
    glLinkProgram(program);
    
    // plus besoin des shaders
    glDetachShader(program, vertex_shader);
    glDeleteShader(vertex_shader);
    glDetachShader(program, fragment_shader);
    glDeleteShader(fragment_shader);
  
    // verifier les erreurs
    if(program_errors(program)) 
        return -1;
    
    // pour etre coherent avec les autres fonctions de creation, active l'objet gl qui vient d'etre cree.
    glUseProgram(program);
    return 0;
}

int reload_program_definitions( const GLuint program, const char *filename, const char *definitions )
{
    std::string common_source= read(filename);
    std::string vertex_source= prepare_source(common_source, std::string(definitions).append("#define VERTEX_SHADER\n"));
    std::string fragment_source= prepare_source(common_source, std::string(definitions).append("#define FRAGMENT_SHADER\n"));
    
    return reload_program_sources(program, vertex_source, fragment_source);
}

int reload_program( const GLuint program, const char *filename )
{
    return reload_program_definitions(program, filename, "");
}

GLuint read_program_definitions( const char *filename, const char *definitions )
{
    GLuint program= glCreateProgram();
    if(reload_program_definitions(program, filename, definitions) < 0)
    {
        glDeleteProgram(program);
        program= 0;
    }
    
    return program;
}

GLuint read_program( const char *filename )
{
    return read_program_definitions(filename, "");
}

} // namespace


// application
void reload_program( )
{
    program_log.clear();
    if(program == 0)
    {
        program= tools::read_program(program_filename);
        program_failed= (program == 0);
    }
    else
    {
        program_failed= (tools::reload_program(program, program_filename) < 0);
    }
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
            {
                //~ label(widgets, "[error] program '%s'...", program_filename);
                label(widgets, "%s", program_log.c_str());
            }
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
