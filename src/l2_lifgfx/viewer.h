
#ifndef VIEWER_H
#define VIEWER_H

#define GLEW_NO_GLU
#include <GL/glew.h>

#include "window.h"
#include "program.h"
#include "buffer.h"
#include "texture.h"

#include "mesh.h"
#include "wavefront.h"

#include "vec.h"
#include "mat.h"
#include "orbiter.h"



struct Viewer
{
    GLuint program;
    orbiter camera;

    GLuint texture;
    mesh cube;
    GLuint cube_vao;
};


//! Initialise tout : compile les shaders et construit le programme + les buffers + le vertex array.
//! renvoie -1 en cas d'erreur.
int init(Viewer& v);

//! La fonction d'affichage
int draw(Viewer& v);

//! Libere tout
int quit(Viewer& v);

#endif
