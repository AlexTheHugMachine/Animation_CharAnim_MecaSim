
//! \file tuto9_materials.cpp utilisation d'un shader 'utilisateur' pour afficher un objet Mesh et ses matieres.

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
        m_objet= read_mesh("data/robot.obj");
        if(m_objet.materials().count() == 0)
            // pas de matieres, pas d'affichage
            return -1;
        printf("%d materials.\n", m_objet.materials().count());
        
        // etape 1 : creer le shader program
        m_program= read_program("tutos/tuto9_materials.glsl");
        program_print_errors(m_program);
        
        // recupere les matieres.
        // le shader declare un tableau de 16 matieres
        m_colors.resize(16);
        
        // copier les matieres utilisees
        const Materials& materials= m_objet.materials();
        assert(materials.count() <= int(m_colors.size()));
        for(int i= 0; i < materials.count(); i++)
            m_colors[i]= materials.material(i).diffuse;
        
        // placer la camera
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        camera().lookat(pmin, pmax);
        
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
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // dessiner m_objet avec le shader program
        // configurer le pipeline 
        glUseProgram(m_program);
        
        // configurer le shader program
        // . recuperer les transformations
        Transform model= RotationY(global_time() / 40);
        Transform view= camera().view();
        Transform projection= camera().projection(window_width(), window_height(), 45);
        
        // . composer les transformations : model, view et projection
        Transform mv= view * model;
        Transform mvp= projection * mv;
        
        // . parametrer le shader program :
        //   . transformation : la matrice declaree dans le vertex shader s'appelle mvpMatrix
        program_uniform(m_program, "mvpMatrix", mvp);
        program_uniform(m_program, "mvMatrix", mv);
        
        //   . ou, directement en utilisant openGL :
        //   int location= glGetUniformLocation(program, "mvpMatrix");
        //   glUniformMatrix4fv(location, 1, GL_TRUE, mvp.buffer());
        
        // . parametres "supplementaires" :
        //   . couleur diffuse des matieres, cf la declaration 'uniform vec4 materials[];' dans le fragment shader
        int location= glGetUniformLocation(m_program, "materials");
        glUniform4fv(location, m_colors.size(), &m_colors[0].r);
        
        // go !
        // indiquer quels attributs de sommets du mesh sont necessaires a l'execution du shader.
        // tuto9_materials.glsl n'utilise que position et material_index. les autres de servent a rien.
        // 1 draw pour tous les triangles de l'objet.
        m_objet.draw(m_program, /* use position */ true, /* use texcoord */ false, /* use normal */ true, /* use color */ false, /* use material index*/ true);
        
        return 1;
    }

protected:
    Transform m_model;
    Mesh m_objet;
    Orbiter m_camera;
    GLuint m_texture;
    GLuint m_program;
    std::vector<Color> m_colors;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    
    return 0;
}
