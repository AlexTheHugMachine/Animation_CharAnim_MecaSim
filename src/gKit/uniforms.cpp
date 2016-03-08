
#include <cstdio>

#include <set>

#include "program.h"
#include "uniforms.h"


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

void program_uniform( const GLuint program, const char *uniform, const Color& c )
{
    glUniform4fv( location(program, uniform), 1, &c.r );
}

void program_uniform( const GLuint program, const char *uniform, const Transform& v )
{
    glUniformMatrix4fv( location(program, uniform), 1, GL_TRUE, &v.m[0][0] );
}

void program_use_texture( const GLuint program, const char *sampler, const int unit, const GLuint texture )
{
    int id= location(program, sampler);
    if(id < 0)
        return;
    
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(id, unit);
}
