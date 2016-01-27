
#ifndef _TEXTURE_H
#define _TEXTURE_H

#define GLEW_NO_GLU
#include "GL/glew.h"

#include "image.h"


GLuint make_texture( const int unit, const image& im );
GLuint read_texture( const int unit, const char *filename );

#endif
