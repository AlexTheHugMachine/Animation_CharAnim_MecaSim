
#include <ctype.h>
#include <cstdio>
#include <cstdarg>

#include "program.h"
#include "buffer.h"
#include "texture.h"
#include "text.h"


Text create_text( )
{
    Text text;
    text.font= read_texture(0, "data/font.png");
    text.program= read_program("data/shaders/text.glsl");
    text.vao= make_vertex_format();
    text.ubo= make_buffer(GL_UNIFORM_BUFFER, sizeof(text.buffer), NULL);
    
    clear(text);
    return text;
}

void release_text( Text& text )
{
    glDeleteTextures(1, &text.font);
    glDeleteProgram(text.program);
    glDeleteVertexArrays(1, &text.vao);
    glDeleteBuffers(1, &text.ubo);
}

void clear( Text& text )
{
    for(int y= 0; y < 24; y++)
    for(int x= 0; x < 128; x++)
        text.buffer[y][x]= ' ';
}

void print( Text& text, const int px, const int py, const char *message )
{
    int x= px;
    int y= 23 - py;     // premiere ligne en haut... 
    
    for(int i= 0; message[i] != 0; i++)
    {
        unsigned char c= message[i];
        if(x >= 128 || c == '\n')
        {
            y--;
            x= px;
        }
        if(c == '\n')
            // ne pas afficher le \n
            continue;
        
        //~ if(!isprint(c)) c= ' ';
        
        if(x < 0 || y < 0) break;
        if(x >= 128 || y >= 24) break;
        text.buffer[y][x]= (int) c;
        x++;
    }
}

void printf( Text& text, const int px, const int py, const char *format, ... )
{
    char tmp[24*128] = { 0 };
    
    va_list args;
    va_start(args, format);
    vsnprintf(tmp, sizeof(tmp), format, args);
    va_end(args);
    
    print(text, px, py, tmp);
}


void draw( const Text& text, const int width, const int height )
{
    glBindVertexArray(text.vao);
    glUseProgram(text.program);
    program_use_texture(text.program, "font", 0, text.font);
    
    program_uniform(text.program, "offset", height - 24*16);
    
    // transfere le texte dans l'uniform buffer
    int index= glGetUniformBlockIndex(text.program, "textData");
    glBindBufferBase(GL_UNIFORM_BUFFER, index, text.ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(text.buffer), text.buffer);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

