
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
#include "app.h"        // classe Application a deriver

#include "AnimationCurve.h"
#include "Viewer.h"

class Viewer : public BasicViewer
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

    Mesh cube;
    GLuint cube_texture;

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
};



#endif
