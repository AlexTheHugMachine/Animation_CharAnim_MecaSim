
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
        
        // trie les triangles par matiere et recupere les groupes de triangles utilisant la meme mateire.
        m_groups= m_objet.groups();
        /* remarque : c'est long, donc il vaut mieux le faire une seule fois au debut du programme...
         */
        
        // placer la camera
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        camera().lookat(pmin, pmax);
        
        // creer le shader program, uniquement necessaire pour l'option 2, cf render()
        m_program= read_program("tutos/tuto9_groups.glsl");
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
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // . recuperer les transformations
        Transform model= RotationY(global_time() / 40);
        Transform view= camera().view();
        Transform projection= camera().projection();
        
    #if 1
        // option 1 : avec les utilitaires draw()
        {
            DrawParam pipeline;
            pipeline.model(model).camera(camera());
            
            for(int i= 0; i < int(m_groups.size()); i++)
            {
                pipeline.draw(m_groups[i], m_objet);
            }
        }
    #else
        // option 2 : dessiner m_objet avec le shader program
        {
            // configurer le pipeline 
            glUseProgram(m_program);
            
            // configurer le shader program
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
            
            // afficher chaque groupe
            for(int i= 0; i < int(m_groups.size()); i++)
            {
                const Material& material= m_objet.materials().material(m_groups[i].material_index);
                
                // . parametres "supplementaires" :
                //   . couleur diffuse de la matiere du groupe de triangle
                program_uniform(m_program, "material_color", material.diffuse);
                
                //   . c'est aussi le bon moment pour changer de texture, par exemple...
                // program_use_texture(m_program, "material_texture", ... );
                
                // go !
                // indiquer quels attributs de sommets du mesh sont necessaires a l'execution du shader.
                // tuto9_groups.glsl n'utilise que position. les autres de servent a rien.
                
                // 1 draw par groupe de triangles...
                m_objet.draw(m_groups[i].first, m_groups[i].n, m_program, /* use position */ true, /* use texcoord */ false, /* use normal */ true, /* use color */ false, /* use material index*/ false);
            }
        }
    #endif

        return 1;
    }

protected:
    Transform m_model;
    Mesh m_objet;
    GLuint m_texture;
    GLuint m_program;
    std::vector<TriangleGroup> m_groups;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    
    return 0;
}
