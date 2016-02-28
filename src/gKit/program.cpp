
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <vector>

#include <cstdio>
#include <cassert>

#include "program.h"


//! charge un fichier texte.
static
std::string read( const char *filename )
{
    std::stringbuf source;
    std::ifstream in(filename);
    if(in.good() == false)
        printf("[error] loading program '%s'...\n", filename);
    else
        printf("loading program '%s'...\n", filename);
    
    in.get(source, 0);        // lire tout le fichier, le caractere '\0' ne peut pas se trouver dans le source de shader
    return source.str();
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
GLuint compile_shader( const GLuint program, const GLenum shader_type, const std::string& source )
{
    if(source.size() == 0) 
        return 0;
    
    GLuint shader= glCreateShader(shader_type);
    glAttachShader(program, shader);
    
    const char *sources= source.c_str();
    glShaderSource(shader, 1, &sources, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    return (status == GL_TRUE) ? shader : 0;
}


int reload_program( GLuint program, const char *filename, const char *definitions )
{
    if(program == 0) 
        return -1;

    // supprime les shaders attaches au program
    int shaders_max= 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &shaders_max);
    
    std::vector<GLuint> shaders(shaders_max, 0);
    glGetAttachedShaders(program, shaders_max, NULL, &shaders.front());
    for(int i= 0; i < shaders_max; i++)
    {
        glDetachShader(program, shaders[i]);
        glDeleteShader(shaders[i]);
    }
    
    // prepare les sources
    std::string common_source= read(filename);
    std::string vertex_source= prepare_source(common_source, std::string(definitions).append("#define VERTEX_SHADER\n"));
    std::string fragment_source= prepare_source(common_source, std::string(definitions).append("#define FRAGMENT_SHADER\n"));

    // cree et compile un vertex shader et un fragment shader
    GLuint vertex_shader= compile_shader(program, GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader= compile_shader(program, GL_FRAGMENT_SHADER, fragment_source);
    // linke les shaders
    glLinkProgram(program);
    
    // verifie les erreurs
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) 
    {
        if(vertex_shader == 0)
            printf("[error] compiling vertex shader...\n%s\n", definitions);
        if(fragment_shader == 0)
            printf("[error] compiling fragment shader...\n%s\n", definitions);
        printf("[error] linking program %u '%s'...\n", program, filename);
        return -1;
    }
    
    // pour etre coherent avec les autres fonctions de creation, active l'objet gl qui vient d'etre cree.
    glUseProgram(program);
    return 0;
}

GLuint read_program( const char *filename, const char *definitions )
{
    GLuint program= glCreateProgram();
    reload_program(program, filename, definitions);
    return program;
}


static
void print_line( std::string& errors, const char *source, const int begin_id, const int line_id )
{
    int line= 0;
    char last= '\n';
    for(unsigned int i= 0; source[i] != 0; i++)
    {
        if(line > line_id)
            break;

        if(last == '\n')
        {
            line++;
            if(line >= begin_id && line <= line_id)
            {
                errors.append("  ");
                errors.push_back('0' + (line / 1000) % 10);
                errors.push_back('0' + (line / 100) % 10);
                errors.push_back('0' + (line / 10) % 10);
                errors.push_back('0' + (line / 1) % 10);
                errors.append("  ");
            }
        }
        
        if(line >= begin_id && line <= line_id)
            errors.push_back(source[i]);
        last= source[i];
    }
}

static
void print_errors( std::string& errors, const char *log, const char *source )
{
    printf("[error log]\n%s\n", log);
    int last_string= -1;
    int last_line= -1;
    for(int i= 0; log[i] != 0; i++)
    {
        // recupere la ligne assiciee a l'erreur
        int string_id= 0, line_id= 0, position= 0;
        if(sscanf(&log[i], "%d ( %d ) : %n", &string_id, &line_id, &position) == 2        // nvidia syntax
        || sscanf(&log[i], "%d : %d (%*d) : %n", &string_id, &line_id, &position) == 2  // mesa syntax
        || sscanf(&log[i], "ERROR : %d : %d : %n", &string_id, &line_id, &position) == 2  // ati syntax
        || sscanf(&log[i], "WARNING : %d : %d : %n", &string_id, &line_id, &position) == 2)  // ati syntax
        {
            if(string_id != last_string || line_id != last_line)
            {
                // extrait la ligne du source...
                errors.append("\n");
                print_line(errors, source, last_line +1, line_id);
                errors.append("\n");
            }
        }
        // et affiche l'erreur associee...
        for(i+= position; log[i] != 0; i++)
        {
            errors.push_back(log[i]);
            if(log[i] == '\n')
                break;
        }
        
        last_string= string_id;
        last_line= line_id;
    }
    errors.append("\n");
    print_line(errors, source, last_line +1, 1000);
    errors.append("\n");
}

int program_format_errors( const GLuint program, std::string& errors )
{
    errors.clear();
    
    if(program == 0)
        return -1;
    
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_TRUE) 
        return 0;
    
    // recupere les shaders
    int shaders_max= 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &shaders_max);
    
    std::vector<GLuint> shaders(shaders_max, 0);
    glGetAttachedShaders(program, shaders_max, NULL, &shaders.front());
    for(int i= 0; i < shaders_max; i++)
    {
        GLint value;
        glGetShaderiv(shaders[i], GL_COMPILE_STATUS, &value);
        if(value == GL_FALSE)
        {
            // recupere les erreurs
            glGetShaderiv(shaders[i], GL_INFO_LOG_LENGTH, &value);
            std::vector<char>log(value+1, 0);
            glGetShaderInfoLog(shaders[i], log.size(), NULL, &log.front());
            
            // recupere le source
            glGetShaderiv(shaders[i], GL_SHADER_SOURCE_LENGTH, &value);
            std::vector<char> source(value+1, 0);
            glGetShaderSource(shaders[i], source.size(), NULL, &source.front());
            
            glGetShaderiv(shaders[i], GL_SHADER_TYPE, &value);
            errors.append("[error] compiling ");
            if(value == GL_VERTEX_SHADER)
                errors.append("vertex shader...\n");
            else if(value == GL_FRAGMENT_SHADER)
                errors.append("fragment shader...\n");
            else
                errors.append("shader...\n");
            
            // formatte les erreurs
            print_errors(errors, &log.front(), &source.front());
        }
    }
    
    return 0;
}

int program_print_errors( const GLuint program )
{
    std::string errors;
    int code= program_format_errors(program, errors);
    if(errors.size() > 0) 
        printf("%s\n", errors.c_str());
    return code;
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
