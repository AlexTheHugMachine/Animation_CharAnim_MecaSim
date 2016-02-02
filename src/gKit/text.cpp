
#include <ctype.h>
#include <stdio.h>

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
    text_clear(text);
    return text;
}

void release_text( Text& text )
{
    glDeleteTextures(1, &text.font);
    glDeleteProgram(text.program);
    glDeleteVertexArrays(1, &text.vao);
    glDeleteBuffers(1, &text.ubo);
}

void text_clear( Text& text )
{
    for(int y= 0; y < 24; y++)
    for(int x= 0; x < 128; x++)
        text.buffer[y][x]= ' ';
}

void text_print( Text& text, const int px, const int py, const char *message )
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

void draw_text( const Text& text, const int width, const int height )
{
    glBindVertexArray(text.vao);
    glUseProgram(text.program);
    program_use_texture(text.program, "font", 0, text.font);
    
    program_set_int(text.program, "offset", height - 24*16);
    
    // transfere le texte dans l'uniform buffer
    int index= glGetUniformBlockIndex(text.program, "textData");
    glBindBufferBase(GL_UNIFORM_BUFFER, index, text.ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(text.buffer), text.buffer);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

