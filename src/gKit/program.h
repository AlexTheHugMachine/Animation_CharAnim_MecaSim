
#ifndef _PROGRAM_H
#define _PROGRAM_H

#define GLEW_NO_GLU
#include "GL/glew.h"

#include "vec.h"
#include "mat.h"


//! cree un shader program. charge les sources d'un vertex shader et d'un fragment shader.
GLuint read_program( const char *vertex, const char *fragment );
GLuint read_program_definitions( const char *vertex, const char *fragment, const char *definitions );

//! renvoie l'indice d'un attribut declare dans le vertex shader.
GLint program_attribute( const GLuint program, const char *attribute );

//! affecte une valeur a un uniform du shader program.
void program_set_uint( const GLuint program, const char *uniform, const unsigned int v );
void program_set_int( const GLuint program, const char *uniform, const int v );
void program_set_float( const GLuint program, const char *uniform, const float v );
void program_set_vec2( const GLuint program, const char *uniform, const vec2& v );
void program_set_vec3( const GLuint program, const char *uniform, const vec3& v );
void program_set_vec4( const GLuint program, const char *uniform, const vec4& v );
void program_set_mat4( const GLuint program, const char *uniform, const mat4& v );

//! configure le pipeline et le shader program pour utiliser une texture.
void program_use_texture( const GLuint program, const char *sampler, const int unit, const GLuint texture );

#endif
