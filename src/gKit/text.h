
#ifndef _TEXT_H
#define _TEXT_H 

#define GLEW_NO_GLU
#include "GL/glew.h"


//! \addtogroup application 
///@{

//! \file
//! console texte.

//! representation d'une console texte, dimension fixe, 24 lignes de 128 colonnes.
struct Text
{
    int buffer[24][128];
    GLuint font;        //!< texture contenant les caracteres.
    GLuint program;     //!< shader pour afficher le texte.
    GLuint vao;         //!< vertex array object.
    GLuint ubo;         //!< uniform buffer object, pour transferrer le texte a afficher
};

//! cree une console.
Text create_text( );
//! detruit une console.
void release_text( Text& text );

//! efface le contenu de la console.
void clear( Text& text );
//! affiche un caractere c sur un fond background.
void print_background( Text& text, const int x, const int y, const int background, const char c );
//! affiche un caractere c sur un fond par defaut.
void print_background( Text& text, const int x, const int y, const char *message );
//! affiche un texte a la position x, y. 
void print( Text& text, const int x, const int y, const char *message );
//! affiche un texte a la position x, y sur un fond par defaut.
void printf_background( Text& text, const int x, const int y, const char *format, ... );
//! affiche un texte a la position x, y. meme utilisation que printf().
void printf( Text& text, const int x, const int y, const char *format, ... );

//! dessine la console.
void draw( const Text& text, const int width, const int height );

///@}
#endif
