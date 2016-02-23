
#ifndef _TEXTURE_H
#define _TEXTURE_H

#define GLEW_NO_GLU
#include "GL/glew.h"

#include "image.h"


//! cree une texture a partir d'une image \param im.
GLuint make_texture( const int unit, const Image& im );
//! cree une texture a partir d'un fichier \param filename.
GLuint read_texture( const int unit, const char *filename );

//! enregistre le contenu de la fenetre dans un fichier. 
int screenshot( const char *filename );

#endif
