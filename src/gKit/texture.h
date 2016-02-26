
#ifndef _TEXTURE_H
#define _TEXTURE_H

#define GLEW_NO_GLU
#include "GL/glew.h"

#include "image.h"

//! \addtogroup openGL
///@{

//! \file 
//! texture2D openGL.


//! cree une texture a partir d'une image im.
GLuint make_texture( const int unit, const Image& im );
//! cree une texture a partir d'un fichier filename.
GLuint read_texture( const int unit, const char *filename );

//! enregistre le contenu de la fenetre dans un fichier filename nom de l'image a ecrire. doit etre de type .png.
int screenshot( const char *filename );

///@}
#endif
