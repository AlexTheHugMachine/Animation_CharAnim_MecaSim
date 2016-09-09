
//! \file tuto7.cpp reprise de tuto6.cpp mais en derivant la classe App.
// utiliser mesh pour charger un objet .obj
// camera pour le dessiner du point de vue d'une camera + controle de la camera a la souris
// texture : creation a partir d'un fichier, utilisation avec draw(mesh, ...) et destruction avec glDeleteTextures( )

#include <cstdio>
#include "window.h"

#include "mesh.h"
#include "wavefront.h"  // pour charger un objet au format .obj
#include "texture.h"

#include "orbiter.h"
#include "draw.h"        // pour dessiner du point de vue d'une camera
#include "app.h"        // classe Application a deriver


class TP : public App
{
public:
    TP( ) : App(1024, 640) {}
    
    int init( )
    {
        // etape 1 : charger un objet
        m_objet= read_mesh("data/cube.obj");
        
        // etape 2 : creer une camera pour observer l'objet
        // construit l'englobant de l'objet, les extremites de sa boite englobante
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);

        // regle le point de vue de la camera pour observer l'objet
        m_camera.lookat(pmin, pmax);

        // etape 3 : charger une texture a aprtir d'un fichier .bmp, .jpg, .png, .tga, etc, utilise read_image( ) et sdl_image
    /*
        openGL peut utiliser plusieurs textures simultanement pour dessiner un objet, il faut les numeroter.
        une texture et ses parametres sont selectionnes sur une unite de texture.
        et ce sont les unites de texture qui sont utilisees pour dessiner un objet.

        l'exemple cree la texture sur l'unite 0 avec les parametres par defaut
     */
        m_texture= read_texture(0, "data/debug2x2red.png");

        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre

        // etape 3 : configuration du pipeline.
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest

        return 0;   // ras, pas d'erreur
    }
    
    int quit( )
    {
          // etape 3 : detruire la description de l'objet
        m_objet.release();
        // et la texture
        glDeleteTextures(1, &m_texture);
        
        return 0;
    }
    
    int draw( )
    {
        // etape 2 : dessiner l'objet avec opengl
        
        // on commence par effacer la fenetre avant de dessiner quelquechose
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // on efface aussi le zbuffer

        // recupere les mouvements de la souris, utilise directement SDL2
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);

        // deplace la camera
        if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            // tourne autour de l'objet
            m_camera.rotation(mx, my);

        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            // approche / eloigne l'objet
            m_camera.move(mx);

        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            // deplace le point de rotation
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        
        ::draw(m_objet, m_camera, m_texture);
        
        return 0;
    }

protected:
    Mesh m_objet;
    GLuint m_texture;
    Orbiter m_camera;
};

int main( )
{
    TP tp;
    tp.run();
    
    return 0;
}
