
#include <ctype.h>
#include <cstdio>
#include <cstdarg>

#include "program.h"
#include "uniforms.h"
#include "image.h"
#include "texture.h"
#include "text.h"


Text create_text( )
{
    Text text;
    
    // charge la fonte
    Image font= read_image("data/font.png");
    
    // modifie la transparence du caractere de fond
    for(unsigned int y= 0; y < 16; y++)
    for(unsigned int x= 0; x < 8; x++)
    {
        const unsigned int starty= 16 *7;
        const unsigned int startx= 8 *2;
        Color color= image_pixel(font, startx + x, starty + y);
        color.a= 0.6;
        image_set_pixel(font, startx + x, starty + y, color);
    }
    
    // cree le curseur
    for(unsigned int y= 0; y < 16; y++)
    for(unsigned int x= 0; x < 8; x++)
    {
        const unsigned int starty= 16 *7;
        const unsigned int startx= 8 *1;
        Color color= x >1 ? make_alpha_color(1, 1, 1, 0.6) : make_alpha_color(1, 1, 1, 1);
        image_set_pixel(font, startx + x, starty + y, color);
    }
    
    text.font= make_texture(0, font);
    release_image(font);
    
    text.program= read_program("data/shaders/text.glsl");
    program_print_errors(text.program);

    clear(text);
    glGenVertexArrays(1, &text.vao);
    glGenBuffers(1, &text.ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, text.ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(text.buffer), text.buffer, GL_STATIC_DRAW);
    
    return text;
}

void release_text( Text& text )
{
    release_program(text.program);
    glDeleteVertexArrays(1, &text.vao);
    glDeleteBuffers(1, &text.ubo);
    glDeleteTextures(1, &text.font);
}

void clear( Text& text )
{
    for(int y= 0; y < 24; y++)
    for(int x= 0; x < 128; x++)
        text.buffer[y][x]= ' ';
}


static
void print( Text& text, const int px, const int py, const int background, const char *message )
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
        if(c == '\n') continue;  // ne pas afficher le \n
        
        if(x < 0 || y < 0) break;
        if(x >= 128 || y >= 24) break;
        
        //~ if(!isprint(c)) c= ' ';
        text.buffer[y][x]= (int) c | (background << 8);
        x++;
    }
}


void print_background( Text& text, const int px, const int py, const int background, const char c )
{
    int x= px;
    int y= 23 - py;     // premiere ligne en haut... 
    if(x < 0 || y < 0) return;
    if(x >= 128 || y >= 24) return;
    //~ if(!isprint(c)) return;
    
    text.buffer[y][x]= (int) c | (background << 8);
}

void print_background( Text& text, const int px, const int py, const char *message )
{
    print(text, px, py, 2, message);
}

void print( Text& text, const int px, const int py, const char *message )
{
    print(text, px, py, 0, message);
}

void printf_background( Text& text, const int px, const int py, const char *format, ... )
{
    char tmp[24*128+1] = { 0 };
    
    va_list args;
    va_start(args, format);
    vsnprintf(tmp, sizeof(tmp), format, args);
    va_end(args);
    
    tmp[24*128]= 0;
    print(text, px, py, 2, tmp);
}

void printf( Text& text, const int px, const int py, const char *format, ... )
{
    char tmp[24*128+1] = { 0 };
    
    va_list args;
    va_start(args, format);
    vsnprintf(tmp, sizeof(tmp), format, args);
    va_end(args);
    
    tmp[24*128]= 0;
    print(text, px, py, 0, tmp);
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
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

