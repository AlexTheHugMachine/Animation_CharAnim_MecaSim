#ifndef VIEWER_H
#define VIEWER_H


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

#include "AnimationCurve.h"
#include "pacman_core/Jeu.h"



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

    //! Menu d aide
    void help();

    //! Mise a jour (par exemple des Mesh)
    int update( const float time, const float delta );

    
protected:

    /// Camera et contexte OpenGL
    Orbiter m_camera;
    DrawParam gl;
    
    /// Trajectoire pour l animation
    AnimationCurve m_anim;
    
    /// Pacman
    Jeu m_pacman;
    
    /// Booleens pour choix du type d affichage
    bool mb_cullface;
    bool mb_wireframe;
    bool b_draw_grid;
    bool b_draw_axe;
    bool b_draw_animation;
    
    /// Declaration des Mesh
    Mesh m_axe;
    Mesh m_grid;
    Mesh m_cube;
    Mesh m_quad;

    /// Declaration des Textures
    GLuint m_cube_texture;
    GLuint m_quad_texture;
    GLuint m_tex_mur;
    GLuint m_tex_pacman;
    GLuint m_tex_fantome;
    GLuint m_tex_pastille;

    /// Declaration des fonction de creation de Mesh du type init_votreObjet()
    void init_axe();
    void init_grid();
    void init_cube();
    void init_quad();
    
    /// Transformations
    Transform Tquad;

    /// Declaration des fonctions d affichage du type
    /// draw_votreObjet(const Transform& T)    
    void draw_axe(const Transform& T);
    void draw_cube(const Transform& T, unsigned int tex);
    void draw_pacman(const Transform& T);
  
    /// Gestion de la camera et des lumieres
    void manageCameraLight();

};

#endif
