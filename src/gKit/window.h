
#ifndef _WINDOW_H
#define _WINDOW_H

#include "SDL2/SDL.h"

typedef SDL_Window *Window;

//! creation d'une fenetre pour l'application.
Window create_window( const int width= 512, const int height= 512 );
void release_window( Window w );

//! cree et configure un contexte opengl
typedef SDL_GLContext Context;

Context create_context( Window window, const int major= 3, const int minor= 2 );
void release_context( Context context );

int window_width( );
int window_height( );

int key_state( const SDL_Keycode key );
void clear_key_state( const SDL_Keycode key );

SDL_KeyboardEvent key_event( );
void clear_key_event( );
SDL_MouseButtonEvent button_event( );
void clear_button_event( );
SDL_MouseWheelEvent wheel_event( );
void clear_wheel_event( );
SDL_TextInputEvent text_event( );
void clear_text_event( );

//! fonction principale.
int run( Window w );

#endif
