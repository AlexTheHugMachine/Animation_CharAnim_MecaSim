
#include <fstream>
#include <sstream>
#include <string>
#include <set>

#include <cstdio>

#include "program.h"


//! charge un fichier texte.
static
std::string read( const char *filename )
{
    std::stringbuf source;
    std::ifstream in(filename);
    if(in.good() == false)
        printf("[error] loading shader '%s'...\n", filename);
    else
        printf("loading shader '%s'...\n", filename);
    
    in.get(source, 0);        // lire tout le fichier, le caractere '\0' ne peut pas se trouver dans le source de shader
    return source.str();
}

//! affiche les erreurs de compilation de shader.
static
bool shader_errors( const GLuint shader )
{
    if(shader == 0) 
        return true;
    
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_TRUE) 
        return false;
    
    char errors[4096];
    glGetShaderInfoLog(shader, sizeof(errors) -1, NULL, errors);
    printf("[shader errors]\n%s\n", errors);
    return true;
}

//! affiche les erreurs de link du programme.
static
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
    printf("[program errors]\n%s\n", errors);
    return true;
}

static 
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
                printf("[error] found several #version directives. failed.\n");
                return std::string();
            }
        }
    }
    else
    {
        printf("[error] no #version directive found. failed.\n");
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

static
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
    if(shader_errors(shader))
    {
        glDeleteShader(shader);
        shader= 0;
    }
    
    return shader;
}

#if 0
std::string program_definition( const char *what, const char *value )
{
    return std::string("#define ").append(what).append(" ").append(value).append("\n");
}

std::string program_definition( const char *what)
{
    return std::string("#define ").append(what).append("\n");
}
#endif

static
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


int reload_program_definitions( const GLuint program, const char *vertex_filename, const char *fragment_filename, const char *definitions )
{
    if(program == 0)
        return -1;
    
    std::string vertex_source= prepare_source(read(vertex_filename), definitions);
    std::string fragment_source= prepare_source(read(fragment_filename), definitions);
    return reload_program_sources(program, vertex_source, fragment_source);
}

GLuint read_program_definitions( const char *vertex_filename, const char *fragment_filename, const char *definitions )
{
    GLuint program= glCreateProgram();
    if(reload_program_definitions(program, vertex_filename, fragment_filename, "") < 0)
    {
        glDeleteProgram(program);
        program= 0;
    }
    
    return program;
}

GLuint read_program( const char *vertex_filename, const char *fragment_filename )
{
    return read_program_definitions(vertex_filename, fragment_filename, "");
}


GLint program_attribute( const GLuint program, const char *attribute )
{
    if(program == 0) 
        return -1;
    
    GLint location= glGetAttribLocation(program, attribute);
    if(location < 0)
    {
        char error[1024]= { 0 };
        sprintf(error, "attribute('%s'): not found.\n", attribute);
        
        static std::set<std::string> log;
        if(log.insert(error).second == true) 
            // pas la peine d'afficher le message 60 fois par seconde...
            printf("%s\n", error); 
        
        return -1; 
    }
    
    return location;
}


static 
int location( const GLuint program, const char *uniform )
{
    if(program == 0) 
        return -1;
    
    // recuperer l'identifiant de l'uniform dans le program
    GLint location= glGetUniformLocation(program, uniform);
    if(location < 0)
    {
        char error[1024]= { 0 };
        sprintf(error, "uniform('%s'): not found.", uniform); 
        
        static std::set<std::string> log;
        if(log.insert(error).second == true) 
            // pas la peine d'afficher le message 60 fois par seconde...
            printf("%s\n", error); 
        
        return -1; 
    }
    
#ifndef GK_RELEASE
    // verifier que le program est bien en cours d'utilisation, ou utiliser glProgramUniform, mais c'est gl 4
    GLuint current;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *) &current);
    if(current != program)
    {
        printf("invalid shader program %u...\n", current);
        glUseProgram(program);
    }
#endif
    
    return location;
}


void program_uniform( const GLuint program, const char *uniform, const unsigned int v )
{
    glUniform1ui( location(program, uniform), v );
}

void program_uniform( const GLuint program, const char *uniform, const int v )
{
    glUniform1i( location(program, uniform), v );
}

void program_uniform( const GLuint program, const char *uniform, const float v )
{
    glUniform1f( location(program, uniform), v );
}

void program_uniform( const GLuint program, const char *uniform, const vec2& v )
{
    glUniform2fv( location(program, uniform), 1, &v.x );
}

void program_uniform( const GLuint program, const char *uniform, const vec3& v )
{
    glUniform3fv( location(program, uniform), 1, &v.x );
}

void program_uniform( const GLuint program, const char *uniform, const Point& a )
{
    glUniform3fv( location(program, uniform), 1, &a.x );
}

void program_uniform( const GLuint program, const char *uniform, const Vector& v )
{
    glUniform3fv( location(program, uniform), 1, &v.x );
}

void program_uniform( const GLuint program, const char *uniform, const vec4& v )
{
    glUniform4fv( location(program, uniform), 1, &v.x );
}

void program_uniform( const GLuint program, const char *uniform, const Transform& v )
{
    glUniformMatrix4fv( location(program, uniform), 1, GL_TRUE, &v.m[0][0] );
}

void program_use_texture( const GLuint program, const char *sampler, const int unit, const GLuint texture )
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    program_uniform(program, sampler, unit);
}
