
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

    void help();

    static Viewer& singleton() { assert(s_singleton); return *s_singleton; }
    static int singleton_draw() { return singleton().draw(); }
protected:
    GLuint program;
    Orbiter camera;

    Mesh axe;
    Mesh grid;
    Mesh cube;
    Mesh sphere;
    Mesh cylinder;
    Mesh terrain;

    bool b_draw_grid;
    bool b_draw_axe;

    GLuint texture;

    void init_axe();
    void init_cube();
    void init_grid();
    void init_sphere();
    void init_cylinder();
    void init_terrain();

    void draw_plane(const Transform& T);

    static Viewer* s_singleton;
};



#endif
