
//! \file tuto2GL.cpp le premier shader, afficher un triangle et le deplacer...

#include <fstream>
#include <sstream>
#include <string>

#include "glcore.h"

#include "window.h"


/* une application opengl est composee de plusieurs composants :
    1. une fenetre pour voir ce que l'on dessine
    2. un contexte openGL pour dessiner
    3. 3 fonctions : 
        init( ) pour creer les objets que l'on veut dessiner, 
        draw( ) pour les afficher / dessiner
        quit( ) pour detruire les objets openGL crees dans init( ), a la fermeture de l'application
        
    ces 3 fonctions sont appelees dans le main.
    
    draw( ) est un peu a part, elle est appellee par la fonction run( ) qui traite les evenements clavier, souris, clicks, etc,
    ca permet de reagir a ces evenements et de modifier / deplacer / animer ce que l'on dessine.
    
    le shader sera compile dans init( ), utilise dans draw( ) et detruit a la fin de l'application dans quit( ).
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
int init( )
{
    // creer un objet openGL : vertex array object
    glGenVertexArrays(1, &vao);
    
    // charger le source du vertex shader
    std::string vertex_source= read("tutos/tuto2GL_vertex.glsl");
    // creer un objet openGL : vertex shader 
    vertex_shader= glCreateShader(GL_VERTEX_SHADER);
    
    // preparer les chaines de caracteres pour compiler le shader
    const char *vertex_strings[]= { vertex_source.c_str() };
    glShaderSource(vertex_shader, 1, vertex_strings, NULL);
    // compiler les sources
    glCompileShader(vertex_shader);
    
    // pareil pour le fragment shader
    std::string fragment_source= read("tutos/tuto2GL_fragment.glsl");
    fragment_shader= glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragment_strings[]= { fragment_source.c_str() };
    glShaderSource(fragment_shader, 1, fragment_strings, NULL);
    glCompileShader(fragment_shader);

    // creer le program et linker les 2 shaders
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
        printf("[error] linking pogram...\n");
    
    // renvoyer 0 ras, pas d'erreur, sinon renvoyer -1
    if(status == GL_TRUE)
        return 0;
    else 
        return -1;
}


// destruction des objets openGL
int quit( )
{
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    return 0;   // ras, pas d'erreur
}


// affichage
int draw( )
{
    // effacer la fenetre : copier la couleur par defaut dans tous les pixels de la fenetre
    glClearColor(0.2, 0.2, 0.2, 1);    // definir la couleur par defaut
    glClear(GL_COLOR_BUFFER_BIT);   // "effacer"
    
    // configurer le pipeline, selectionner le shader program a utiliser
    glUseProgram(program);
    
    // configurer le pipeline, selectionner le vertex array a utiliser
    glBindVertexArray(vao);
    
    // dessiner 1 triangle, indices gl_VertexID de 0 a 3
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    return 1;   // on continue, renvoyer 0 pour sortir de l'application
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
