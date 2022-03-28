
//! \file tuto9.cpp utilisation d'un shader 'utilisateur' pour afficher un objet Mesh

#include "mat.h"
#include "mesh.h"
#include "wavefront.h"

#include "orbiter.h"
#include "program.h"
#include "uniforms.h"
#include "draw.h"

#include "app_camera.h"        // classe Application a deriver


class TP : public AppCamera
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : AppCamera(1024, 640) {}
    
    int init( )
    {
        // charge un obet
        m_objet= read_mesh("data/cube.obj");
        
        // parametre la camera pour observer l'objet
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        camera().lookat(pmin, pmax);
        
        // cree le shader program
        m_program= read_program("tutos/tuto9_color.glsl");
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
        // detruire le shader program
        release_program(m_program);
        // et l'objet...
        m_objet.release();
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // configurer le pipeline 
        glUseProgram(m_program);
        
        // configurer le shader program
        // . recuperer les transformations
        Transform model= RotationX(global_time() / 20);         // fait tourner l'objet sur lui meme en fonction du temps
        Transform view= camera().view();
        Transform projection= camera().projection();
        
        // . composer les transformations : model, view et projection
        Transform mvp= projection * view * model;
        
        // . parametrer le shader program :
        //   . transformation : la matrice declaree dans le vertex shader s'appelle 'mvpMatrix'
        program_uniform(m_program, "mvpMatrix", mvp);
        
        // . parametre "supplementaire" :
        //   . couleur des pixels, la couleur declaree dans le fragment shader s'appelle 'color'
        program_uniform(m_program, "color", Color(1, 1, 0, 1));
        
        // go !
        // indiquer quels attributs de sommets du mesh sont necessaires a l'execution du shader.
        // tuto9_color.glsl n'utilise que position. les autres de servent a rien.
        m_objet.draw(m_program, /* use position */ true, /* use texcoord */ false, /* use normal */ false, /* use color */ false, /* use material index*/ false);
        
        return 1;
    }

protected:
    Mesh m_objet;
    Transform m_model;
    GLuint m_program;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    
    return 0;
}
