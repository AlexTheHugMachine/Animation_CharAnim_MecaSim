
#ifndef _BUFFER_H
#define _BUFFER_H

#define GLEW_NO_GLU
#include "GL/glew.h"


GLuint make_buffer( const GLenum target, const int data_size, const void *data );

GLuint make_vertex_format( );
GLuint make_vertex_buffer( const GLuint vao, const GLint attribute, const int size, const GLenum type, const int data_size, const void *data );
GLuint make_index_buffer( const GLuint vao, const int data_size, const void *data );

void release_vertex_format( const GLuint vao );

#endif
