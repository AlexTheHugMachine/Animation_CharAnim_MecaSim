
#ifndef _TEXT_H
#define _TEXT_H 

#define GLEW_NO_GLU
#include "GL/glew.h"


struct text_t
{
    int buffer[24][128];
    GLuint font;
    GLuint program;
    GLuint vao;
    GLuint ubo;
};

text_t create_text( );
void release_text( text_t& text );

void text_clear( text_t& text );
void text_print( text_t& text, const int x, const int y, const char *message );

void draw_text( const text_t& text, const int width, const int height );


#endif
