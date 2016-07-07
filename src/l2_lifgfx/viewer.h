
#ifndef VIEWER_H
#define VIEWER_H

#define GLEW_NO_GLU
#include <GL/glew.h>

#include "window.h"
#include "program.h"
#include "buffer.h"
#include "texture.h"

#include "mesh.h"

#include "vec.h"
#include "mat.h"
#include "orbiter.h"



class Viewer
{
public:
    Viewer();

    //! Initialise tout : compile les shaders et construit le programme + les buffers + le vertex array.
    //! renvoie -1 en cas d'erreur.
    int init();

    //! La fonction d'affichage
    int draw();

    //! Libere tout
    int quit();

    static Viewer& singleton() { return *s_singleton; }
    static int singleton_draw() { return singleton().draw(); }
protected:
    GLuint program;
    Orbiter camera;

    Mesh axe;
    Mesh cube;

    GLuint texture;

    void init_axe();
    void init_cube();

    static Viewer* s_singleton;
};



#endif
