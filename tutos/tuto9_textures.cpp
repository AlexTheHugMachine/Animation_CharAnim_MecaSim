
//! \file tuto9_textures.cpp utilisation d'un shader 'utilisateur' pour afficher un objet Mesh avec une texture.

#include "mat.h"
#include "mesh.h"
#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "program.h"
#include "uniforms.h"
#include "draw.h"

#include "app.h"        // classe Application a deriver


class TP : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : App(1024, 640) {}
    
    int init( )
    {
        m_objet= read_mesh("data/cube.obj");
        
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);
        
        // lire une texture sur l'unite 0
        m_texture0= read_texture(0, "data/debug2x2red.png");
        // lire une texture sur l'unite 1
        m_texture1= read_texture(1, "data/pacman.png");
        
        // creer le shader program
        m_program= read_program("tutos/tuto9_textures.glsl");
        program_print_errors(m_program);
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest
        
        return 0;   // ras, pas d'erreur
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        // etape 3 : detruire le shader program
        release_program(m_program);
        
        m_objet.release();
        glDeleteTextures(1, &m_texture0);
        glDeleteTextures(1, &m_texture1);
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // deplace la camera
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.move(mx);
        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        
        // etape 2 : dessiner m_objet avec le shader program
        // configurer le pipeline 
        glUseProgram(m_program);
        
        // configurer le shader program
        // . recuperer les transformations
        Transform model= RotationX(global_time() / 20);
        Transform view= m_camera.view();
        Transform projection= m_camera.projection(window_width(), window_height(), 45);
        
        // . composer les transformations : model, view et projection
        Transform mvp= projection * view * model;
        
        // . parametrer le shader program :
        //   . transformation : la matrice declaree dans le vertex shader s'appelle mvpMatrix
        program_uniform(m_program, "mvpMatrix", mvp);
        
        // . parametres "supplementaires" :
        //   . utilisation d'une texture configuree sur l'unite 0, le fragment shader declare "uniform sampler2D texture0;"
        program_use_texture(m_program, "texture0", 0, m_texture0);
        
        //   . utilisation d'une texture configuree sur l'unite 1, le fragment shader declare "uniform sampler2D texture1;" 
        program_use_texture(m_program, "texture1", 1, m_texture1);
        
        // go !
        // indiquer quels attributs de sommets du mesh sont necessaires a l'execution du shader.
        // tuto9_textures.glsl n'utilise que position et texcoord. les autres de servent a rien.
        m_objet.draw(m_program, /* use position */ true, /* use texcoord */ true, /* use normal */ false, /* use color */ false, /* use material index*/ false);
        return 1;
    }
    
protected:
    Mesh m_objet;
    Orbiter m_camera;
    GLuint m_texture0;
    GLuint m_texture1;
    GLuint m_program;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    
    return 0;
}
