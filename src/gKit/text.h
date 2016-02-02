
#ifndef _TEXT_H
#define _TEXT_H 

#define GLEW_NO_GLU
#include "GL/glew.h"


struct Text
{
    int buffer[24][128];
    GLuint font;
    GLuint program;
    GLuint vao;
    GLuint ubo;
};

Text create_text( );
void release_text( Text& text );

void clear( Text& text );
void print( Text& text, const int x, const int y, const char *message );
void printf( Text& text, const int x, const int y, const char *format, ... );

void draw( const Text& text, const int width, const int height );


#endif
