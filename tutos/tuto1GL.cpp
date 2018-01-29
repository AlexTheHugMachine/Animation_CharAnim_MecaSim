
//! \file tuto1GL.cpp application minimaliste openGL3 core

#include <fstream>
#include <sstream>
#include <string>

#include "glcore.h"

#include "SDL2/SDL.h"

/* une application opengl est composee de plusieurs composants :
    1. une fenetre pour voir ce que l'on dessine
    2. un contexte openGL pour dessiner
    3. 3 fonctions : 
        init( ) pour creer les objets que l'on veut dessiner, 
        draw( ) pour les afficher / dessiner
        quit( ) pour detruire les objets openGL crees dans init( ), a la fermeture de l'application
        
    ces 3 fonctions sont appelees dans le main.
 */


// identifiants des shaders
GLuint vertex_shader;
GLuint fragment_shader;
// identifiant du shader program
GLuint program;

// identifiant du vertex array object
GLuint vao;


// utilitaire : charger un fichier texte et renvoyer une chaine de caracteres.
std::string read( const char *filename )
{
    std::stringbuf source;
    std::ifstream in(filename);
    // verifie que le fichier existe
    if(in.good() == false)
        printf("[error] loading program '%s'...\n", filename);
    else
        printf("loading program '%s'...\n", filename);
    
    // lire le fichier, le caractere '\0' ne peut pas se trouver dans le source de shader
    in.get(source, 0);
    // renvoyer la chaine de caracteres
    return source.str();
}


// creation des objets openGL
bool init_program( )
{
    // charger le source du vertex shader
    std::string vertex_source= read("tutos/tuto1GL_vertex.glsl");
    // creer un objet openGL : vertex shader 
    vertex_shader= glCreateShader(GL_VERTEX_SHADER);
    
    // preparer les chaines de caracteres pour compiler le shader
    const char *vertex_strings[]= { vertex_source.c_str() };
    glShaderSource(vertex_shader, 1, vertex_strings, NULL);
    // compiler les sources
    glCompileShader(vertex_shader);
    
    // pareil pour le fragment shader
    std::string fragment_source= read("tutos/tuto1GL_fragment.glsl");
    fragment_shader= glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragment_strings[]= { fragment_source.c_str() };
    glShaderSource(fragment_shader, 1, fragment_strings, NULL);
    glCompileShader(fragment_shader);

    // creer un object openGL : shader program 
    program= glCreateProgram();
    // inclure les 2 shaders dans le program
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    // linker les shaders
    glLinkProgram(program);
    
    // verifier que tout c'est bien passe, si les shaders ne se sont pas compiles correctement, le link du program va echouer.
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) 
    {
        printf("[error] compiling shaders / linking pogram...\n");
        return false;
    }
    
    // ok, pas d'erreur
    return true;
}

bool init_vao( )
{
    // creer un objet openGL : vertex array object
    glGenVertexArrays(1, &vao);
    
    return true;
}

bool init( )
{
    // init shader program
    if(!init_program())
        return false;

    // init vao
    if(!init_vao())
        return false;
    
    // options globales du pipeline
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // dessiner dans les images associees a la fenetre
    glDrawBuffer(GL_BACK);      // dessiner dans l'image non affichee de la fenetre
    
/* cf l'option SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); dans main() qui demande la creation d'au moins 2 images, GL_FRONT et GL_BACK.
    GL_FRONT est l'image affichee par la fenetre, on peut dessiner dans GL_BACK sans perturber l'affichage. lorsque le dessin est fini, on echange les 2 images...
    
    remarque : si une seule image est associee a la fenetre, 
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0); 
        utiliser :
        glDrawBuffer(GL_FRONT); 
 */
    
    glViewport(0, 0, 1024, 640);        // definir la region (x, y, largeur, hauteur) de la fenetre dans laquelle dessiner
    
    glClearColor(0.2, 0.2, 0.2, 1);     // definir la couleur par defaut (gris)
    
    glDisable(GL_DEPTH_TEST);           // pas de test sur la profondeur
    glDisable(GL_CULL_FACE);            // pas de test sur l'orientation
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // dessiner tous les pixels du triangle
    
    return true;
}


// destruction des objets openGL
void quit( )
{
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
}


// affichage
void draw( )
{
    // effacer la fenetre : copier la couleur par defaut dans tous les pixels de la fenetre
    // cf init(): glClearColor(0.2, 0.2, 0.2, 1); 
    glClear(GL_COLOR_BUFFER_BIT);
    
    // configurer le pipeline, selectionner le vertex array a utiliser
    glBindVertexArray(vao);

    // configurer le pipeline, selectionner le shader program a utiliser
    glUseProgram(program);
    
    // pas d'uniforms dans le shader program, donc rien a faire...
    
    // dessiner 1 triangle, soit 3 indices (gl_VertexID varie de 0 a 3)
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


int main( int argc, char **argv )
{
    // init sdl
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("[error] SDL_Init() failed:\n%s\n", SDL_GetError());
        return 1;       // erreur lors de l'init de sdl2
    }

    // enregistre le destructeur de sdl
    atexit(SDL_Quit);

    // etape 1 : creer la fenetre, utilise sdl
    SDL_Window *window= SDL_CreateWindow("gKit", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 640, SDL_WINDOW_OPENGL);
    if(window == NULL)
    {
        printf("[error] SDL_CreateWindow() failed.\n");
        return 1;       // erreur lors de la creation de la fenetre ou de l'init de sdl2
    }

    // etape 2 : creer un contexte opengl core profile pour dessiner, utilise sdl2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);                               // version 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);               // infos de debug
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);      // version moderne
    
    // dessiner dans une image differente de celle affichee par la fenetre de l'application
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GLContext context= SDL_GL_CreateContext(window);
    if(context == NULL)
    {
        printf("[error] creating openGL context.\n");
        return 1;
    }

    SDL_GL_SetSwapInterval(1);  // attendre l'ecran pour echanger les images affichee et buffer de dessin

#ifndef NO_GLEW
    // initialise les extensions opengl, si necessaire
    glewExperimental= 1;
    GLenum err= glewInit();
    if(err != GLEW_OK)
    {
        printf("[error] loading extensions\n%s\n", glewGetErrorString(err));
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);        
        return 1;       // erreur lors de l'init de glew / import des fonctions opengl
    }

    // purge les erreurs opengl generees par glew !
    while(glGetError() != GL_NO_ERROR) {;}
#endif
    
    // etape 3 : creation des objets 
    if(!init())
    {
        printf("[error] init failed.\n");
        return 1;
    }
    
    // etape 4 : affichage de l'application, tant que la fenetre n'est pas fermee. 
    bool done= false;
    while(!done)
    {
        // gestion des evenements
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                done= true;  // sortir si click sur le bouton de la fenetre
            else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                done= true;  // sortir si la touche esc / echapp est enfoncee
        }
        
        // dessiner
        draw();
        
        // presenter / montrer le resultat, echanger les images associees a la fenetre, GL_FRONT et GL_BACK, cf init()
        SDL_GL_SwapWindow(window);
    }

    // etape 5 : nettoyage
    quit();
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    return 0;
}
