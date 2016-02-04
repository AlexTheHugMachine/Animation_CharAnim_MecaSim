
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


//! cree et compile un shader.
static
GLuint compile_shader( const GLenum shader_type, const char *filename, const std::string& definitions )
{
    std::string file= read(filename);
    if(file.empty()) 
        return 0;
    
    // un peu de gymnastique, #version doit rester sur la premiere ligne, meme si on insere des #define dans le source
    std::string source;
    std::string errors;
    
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
                errors.append("found several #version directives. failed.\n");
        }
    }
    else
        errors.append("no #version directive found. failed.\n");
    
    // reconstruit le source complet
    if(definitions.empty() == false)
    {
        // insere la version
        source.append(version);
        // insere les definitions
        source.append(definitions).append("\n");
        // insere le source
        source.append(file);
    }
    else
    {
        // re-insere la version (supprimee de file)
        source.append(version);
        // insere le source
        source.assign(file);
    }
    
    //~ printf("[compile shader]\n%s\n\n", source.c_str());
    
    // compile 
    const char *sources= source.c_str();
    GLuint shader= glCreateShader(shader_type);
    glShaderSource(shader, 1, &sources, NULL);
    glCompileShader(shader);
    
    if(shader_errors(shader))
    {
        // affiche les erreurs
        if(shader_type == GL_VERTEX_SHADER) printf("[vertex ");
        else if(shader_type == GL_FRAGMENT_SHADER) printf("[fragment ");
        
        printf("shader '%s']\n%s\n%s\n\n", filename, errors.c_str(), source.c_str());
        
        glDeleteShader(shader);
        shader= 0;
    }
    
    return shader;
}


GLuint read_program_definitions( const char *vertex, const char *fragment, const char *definitions )
{
    // creer et compiler un vertex shader et un fragment shader
    GLuint vertex_shader= compile_shader(GL_VERTEX_SHADER, vertex, std::string(definitions).append("#define VERTEX_SHADER\n"));
    if(vertex_shader == 0)
        return 0;
    GLuint fragment_shader= compile_shader(GL_FRAGMENT_SHADER, fragment, std::string(definitions).append("#define FRAGMENT_SHADER\n"));
    if(fragment_shader == 0)
        return 0;
    
    // creer un shader program
    GLuint program= glCreateProgram();
    // associer les shaders au program
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    // linker les shaders pour obtenir un program utilisable
    glLinkProgram(program);
    
    // verifier les erreurs
    if(program_errors(program)) 
    {
        glDeleteProgram(program);
        program= 0;
    }
    
    // plus besoin des shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
  
    // pour etre coherent avec les autres fonctions de creation, active l'objet gl qui vient d'etre cree.
    if(program > 0) 
        glUseProgram(program);
    return program;
}

GLuint read_program( const char *vertex, const char *fragment )
{
    return read_program_definitions(vertex, fragment, "");
}

GLuint read_program( const char *shaders )
{
    return read_program_definitions(shaders, shaders, "");
}

GLuint read_program_definitions( const char *shaders, const char *definitions )
{
    return read_program_definitions(shaders, shaders, definitions);
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
