
#ifndef VIEWER_H
#define VIEWER_H

#define GLEW_NO_GLU
#include <GL/glew.h>

#include "window.h"
#include "program.h"
#include "buffer.h"
#include "texture.h"
#include "mesh.h"
#include "draw.h"
#include "vec.h"
#include "mat.h"
#include "orbiter.h"
#include "app.h"

#include "AnimationCurve.h"


class Viewer : public App
{
public:
    Viewer();

    //! Initialise tout : compile les shaders et construit le programme + les buffers + le vertex array.
    //! renvoie -1 en cas d'erreur.
    int init();

    //! La fonction d'affichage
    int render();

    //! Libere tout
    int quit();

    void help();


protected:

    Orbiter camera;
    DrawParam gl;
    AnimationCurve m_anim;
    bool mb_cullface;
    bool mb_wireframe;

    Mesh axe;
    Mesh grid;
    Mesh cube;
    GLuint cube_texture;

    bool b_draw_grid;
    bool b_draw_axe;
    bool b_draw_animation;
    void init_axe();
    void init_grid();
    void init_cube();


    /* Pour creer un nouvel objet vous devez :
       - declarer ici le Mesh
       - la texture si besoin
       - une fonction init_votreObjet et l'appeller dans la fonction init du .cpp
       - ajouter un appel a l'affichage dans la fonction draw
    */
    Mesh quad;
    GLuint quad_texture;
    void init_quad();

    void manageCameraLight();
};



#endif
