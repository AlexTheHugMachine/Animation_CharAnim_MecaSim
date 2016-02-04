
#ifndef _WIDGETS_H
#define _WIDGETS_H

#include "text.h"


struct Widgets
{
    Text console;
    int px, py;
    
    int focus;
    int fx, fy;
    int mx, my;
    int mb;
    
    int key;
    unsigned int mod;
};

Widgets create_widgets( );
void release_widgets( Widgets& widgets );

void begin( Widgets& widgets );

void label( Widgets& widgets, const int px, const int py, const char *text );
bool button( Widgets& widgets, const int px, const int py, const char *text, int& status );
bool edit( Widgets& widgets, const int px, const int py, const int text_size, char *text );

void end( Widgets& widgets );

void draw( Widgets& widgets, const int width, const int height );

#endif
