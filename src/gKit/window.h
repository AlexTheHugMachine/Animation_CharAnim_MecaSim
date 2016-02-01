
#ifndef _WINDOW_H
#define _WINDOW_H

#include "SDL2/SDL.h"

typedef SDL_Window *window;

//! creation d'une fenetre pour l'application.
window create_window( const int width= 512, const int height= 512 );
void release_window( window w );

//! cree et configure un contexte opengl
typedef SDL_GLContext context;

context create_context( window window, const int major= 3, const int minor= 3 );
void release_context( context context );

int window_width( );
int window_height( );

int key_state( const SDL_Keycode key );
void clear_key_state( const SDL_Keycode key );

//! fonction principale.
int run( window w );

#endif
