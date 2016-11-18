
#ifndef VIEWER_H
#define VIEWER_H


#include "glcore.h"

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

    int update( const float time, const float delta );

protected:

    Orbiter m_camera;
    DrawParam gl;
    AnimationCurve m_anim;
    bool mb_cullface;
    bool mb_wireframe;

    Mesh m_axe;
    Mesh m_grid;
    Mesh m_cube;
    GLuint m_cube_texture;

    bool b_draw_grid;
    bool b_draw_axe;
    bool b_draw_animation;
    void init_axe();
    void init_grid();
    void init_cube();


    /* Pour creer un nouvel objet vous devez :
       1. declarer ici
                le Mesh,
                la texture si besoin,
                une fonction 'init_votreObjet'
        2. Appeller la fonction 'init_votreObjet' dans la fonction 'init' du .cpp
        3. Pour l'affichage, ajouter un appel  gl.draw(votreMesh); dans la fonction 'render' du .cpp
    */
    Mesh m_quad;
    GLuint m_quad_texture;
    void init_quad();
    Transform Tquad;


    void draw_axe(const Transform& T);
    void manageCameraLight();
};



#endif
