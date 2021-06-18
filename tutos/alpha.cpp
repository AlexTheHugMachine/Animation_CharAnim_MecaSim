
#include "wavefront.h"

#include "draw.h"
#include "program.h"
#include "texture.h"
#include "uniforms.h"
#include "app_camera.h"


// utilitaire. creation d'une grille / repere.
Mesh make_grid( const int n= 10 )
{
    Mesh grid= Mesh(GL_LINES);
    
    // grille
    grid.color(White());
    for(int x= 0; x < n; x++)
    {
        float px= float(x) - float(n)/2 + .5f;
        grid.vertex(Point(px, 0, - float(n)/2 + .5f)); 
        grid.vertex(Point(px, 0, float(n)/2 - .5f));
    }

    for(int z= 0; z < n; z++)
    {
        float pz= float(z) - float(n)/2 + .5f;
        grid.vertex(Point(- float(n)/2 + .5f, 0, pz)); 
        grid.vertex(Point(float(n)/2 - .5f, 0, pz)); 
    }
    
    // axes XYZ
    grid.color(Red());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(1, .1, 0));
    
    grid.color(Green());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(0, 1, 0));
    
    grid.color(Blue());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(0, .1, 1));
    
    glLineWidth(2);
    
    return grid;
}


class TP : public AppCamera
{
public:
    // constructeur : donner les dimensions de l'image, version openGL + nombres de samples MSAA (ou 1)
    TP( const int msaa= 1 ) : AppCamera(1024, 640, 4,3, msaa) {}
    
    // creation des objets de l'application
    int init( )
    {
        // decrire un repere / grille 
        m_repere= make_grid(20);
        
        Point pmin, pmax;
        m_repere.bounds(pmin, pmax);
        // parametrer la camera de l'application, renvoyee par la fonction camera()
        camera().lookat(pmin, pmax);
        
        m_objet= read_mesh("data/alpha.obj");
        //~ m_objet= read_mesh("data/robot.obj");
        m_vao= m_objet.create_buffers( /* texcoords */ true, /* normals */ true, /* color */ false, /* material index */ false);
        
        // charge les textures, si necessaire
        Materials& materials= m_objet.materials();
        m_textures.resize(materials.filename_count());
        for(unsigned i= 0; i < m_textures.size(); i++)
            m_textures[i]= read_texture(0, materials.filename(i));
        
        m_program= read_program("tutos/alpha.glsl");
        program_print_errors(m_program);
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest
        
        if(window_msaa() > 1)
        {
            glEnable(GL_MULTISAMPLE);
            glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
            //~ glEnable(GL_SAMPLE_ALPHA_TO_ONE);
        }
        
        return 0;   // ras, pas d'erreur
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        m_objet.release();
        m_repere.release();
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(key_state('r'))
        {
            // recharge le shader a la volee...
            clear_key_state('r');
            reload_program(m_program, "tutos/alpha.glsl");
            program_print_errors(m_program);
        }
        
        glBindVertexArray(m_vao);
        glUseProgram(m_program);
        
        Transform model= Identity();
        Transform view= camera().view();
        Transform projection= camera().projection();
        Transform mv= view * model;
        Transform mvp= projection * mv;
        
        program_uniform(m_program, "mvMatrix", mv);
        program_uniform(m_program, "mvpMatrix", mvp);
        if(m_textures.size())
            program_use_texture(m_program, "alpha", 0, m_textures[0]);
            
        glDrawArrays(GL_TRIANGLES, 0, m_objet.vertex_count());
        
        // dessine aussi le repere, pour le meme point de vue
        draw(m_repere, Identity(), camera());
        
        // continuer...
        return 1;
    }

protected:
    Mesh m_objet;
    Mesh m_repere;
    GLuint m_vao;
    GLuint m_program;
    std::vector<GLuint> m_textures;
};


int main( int argc, char **argv )
{
    // il ne reste plus qu'a creer un objet application et la lancer 
    TP tp(8);
    //~ TP tp(1);
    tp.run();
    
    return 0;
}
