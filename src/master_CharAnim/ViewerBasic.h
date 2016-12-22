
#ifndef VIEWERBASIC_H
#define VIEWERBASIC_H


#include "glcore.h"

#include "window.h"
#include "program.h"
#include "texture.h"
#include "mesh.h"
#include "draw.h"
#include "vec.h"
#include "mat.h"
#include "orbiter.h"
#include "app.h"


class ViewerBasic : public App
{
public:
	ViewerBasic();

    //! Initialise tout : compile les shaders et construit le programme + les buffers + le vertex array. renvoie -1 en cas d'erreur.
    int init();

    //! La fonction d'affichage
    int render();

    void help();

	int quit() { return 1;  }

protected:

    Orbiter m_camera;
    DrawParam gl;
    bool mb_cullface;
    bool mb_wireframe;

    Mesh m_axe;
    Mesh m_grid;
    Mesh m_cube;

    bool b_draw_grid;
    bool b_draw_axe;
    void init_axe();
    void init_grid();
    void init_cube();

    Mesh m_quad;
    void init_quad();

    void draw_axe(const Transform& T);
	void draw_grid(const Transform& T);
	void draw_cube(const Transform& T);
	void draw_quad(const Transform& T);

    void manageCameraLight();
};



#endif
