
// "hashed alpha testing" https://casual-effects.com/research/Wyman2017Hashed/

// \todo utiliser la quantification du bruit pour le stabiliser...

// \todo comparer avec "Weighted Blended Order-Independent Transparency" http://jcgt.org/published/0002/02/09/


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
        
        Point grid_pmin, grid_pmax;
        m_repere.bounds(grid_pmin, grid_pmax);
        
        //~ m_objet= read_mesh("data/alpha.obj");   // utiliser alpha.glsl !!
        m_objet= read_mesh("data/robot.obj");   // utiliser alpha_notexture.glsl !!
        
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        
        pmin= min(pmin, grid_pmin);
        pmax= max(pmax, grid_pmax);

        // parametrer la camera de l'application, renvoyee par la fonction camera()
        camera().lookat(pmin, pmax);
        
        // charge les textures, si necessaire
        Materials& materials= m_objet.materials();
        m_textures.resize(materials.filename_count());
        for(unsigned i= 0; i < m_textures.size(); i++)
            m_textures[i]= read_texture(0, materials.filename(i));
        
        // affiche les matieres
        {
            for(int i= 0; i < materials.count(); i++)
            {
                const Material& material= materials.material(i);
                printf("material[%d] '%s' kd %f %f %f ", i, materials.name(i), material.diffuse.r, material.diffuse.g, material.diffuse.b);
                if(material.diffuse_texture != -1)
                    printf("texture '%s'", materials.filename(material.diffuse_texture));
                printf("\n");
            }
        }
        
        m_groups= m_objet.groups();
        m_vao= m_objet.create_buffers( /* texcoords */ true, /* normals */ true, /* color */ false, /* material index */ false);
        
        //
        //~ m_program= read_program("tutos/alpha.glsl");
        m_program= read_program("tutos/alpha_notexture.glsl");
        program_print_errors(m_program);
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest
        
        glEnable(GL_MULTISAMPLE);   // MSAA
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);  // transformer la transparence en visibilite des samples MSAA
        
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
        
        // dessine aussi le repere, pour le meme point de vue
        draw(m_repere, Identity(), camera());
        
        if(key_state('r'))
        {
            // recharge le shader a la volee...
            clear_key_state('r');
            //~ reload_program(m_program, "tutos/alpha.glsl");
            reload_program(m_program, "tutos/alpha_notexture.glsl");
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
        
        // dessine tous les groupes de triangles, tries par matiere
        const Materials& materials= m_objet.materials();
        for(unsigned i= 0; i < m_groups.size(); i++)
        {
            const Material& material= materials.material(m_groups[i].material_index);
            if(material.diffuse_texture != -1)
                program_use_texture(m_program, "alpha", 0, m_textures[material.diffuse_texture]);
            else
                program_use_texture(m_program, "alpha", 0, 0);
            
            //~ printf("group[%u] material %d first %d n %d, texture %d\n", i, m_groups[i].material_index, m_groups[i].first, m_groups[i].n, material.diffuse_texture);
            
            glDrawArrays(GL_TRIANGLES, m_groups[i].first, m_groups[i].n);
        }
        
        if(key_state('s'))
        {
            clear_key_state('s');
            static int id= 1;
            screenshot("alpha", id++);
        }
        
        // continuer...
        return 1;
    }

protected:
    Mesh m_objet;
    Mesh m_repere;
    GLuint m_vao;
    GLuint m_program;
    std::vector<GLuint> m_textures;
    std::vector<TriangleGroup> m_groups;
};


int main( int argc, char **argv )
{
    // il ne reste plus qu'a creer un objet application et la lancer 
    TP tp(8);
    //~ TP tp(1);
    tp.run();
    
    return 0;
}
