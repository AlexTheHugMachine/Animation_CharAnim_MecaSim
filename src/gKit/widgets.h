
#ifndef _WIDGETS_H
#define _WIDGETS_H

#include "text.h"


struct Widgets
{
    Text console;
    int px, py;         // placement des widgets
    
    int focus;          // focus
    int fx, fy;         // position du focus
    
    int mb;             // click
    int mx, my;         // position du click
    
    int key;            // touche
    unsigned int mod;   // touches supplementaires, alt, ctrl, etc.
};


Widgets create_widgets( );
void release_widgets( Widgets& widgets );

void begin( Widgets& widgets );

void begin_line( Widgets& widgets );

void label( Widgets& widgets, const char *format, ... );
bool button( Widgets& widgets, const char *text, int& status );
bool edit( Widgets& widgets, const int text_size, char *text );

void end_line( Widgets& widgets );

void end( Widgets& widgets );

void draw( Widgets& widgets, const int width, const int height );

#endif
