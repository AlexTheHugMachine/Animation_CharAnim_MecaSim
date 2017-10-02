
//! \file mesh_viewer.cpp


#include <algorithm>

#include "mat.h"
#include "mesh_data.h"
#include "mesh_buffer.h"

#include "orbiter.h"
#include "program.h"
#include "uniforms.h"

#include "app_time.h"        // classe Application a deriver


class MeshViewer: public AppTime
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    MeshViewer( const char *file ) : AppTime(1024, 640), m_filename(file) {}
    
    int init( )
    {
        // lit les donnees
        MeshData data= read_mesh_data(m_filename);
        if(data.positions.size() == 0)
            return -1;
        
        printf("data : %d positions, %d texcoords, %d normals, %d triangles\n", 
            (int) data.positions.size(), (int) data.texcoords.size(), (int) data.normals.size(), (int) data.material_indices.size());
        
        // calcule l'englobant 
        Point pmin, pmax;
        bounds(data, pmin, pmax);
        m_camera.lookat(pmin, pmax);

        // recalcule les normales des sommets, si necessaire
        if(data.normals.size() == 0)
            normals(data);
        
        printf("normals : %d positions, %d texcoords, %d normals, %d triangles\n", 
            (int) data.positions.size(), (int) data.texcoords.size(), (int) data.normals.size(), (int) data.material_indices.size());
        
        // construit les buffers
        m_mesh= buffers(data);
        // conserve le nombre de sommets et d'indices
        m_vertex_count= m_mesh.positions.size();
        m_index_count= m_mesh.indices.size();
        
        printf("mesh : %d positions, %d texcoords, %d normals, %d indices\n", 
            (int) m_mesh.positions.size(), (int) m_mesh.texcoords.size(), (int) m_mesh.normals.size(), (int) m_mesh.indices.size());
        
        // construit les buffers openGL
        size_t size= m_mesh.vertex_buffer_size() + m_mesh.texcoord_buffer_size() + m_mesh.normal_buffer_size();
        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
        
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        // transfere les positions des sommets
        size_t offset= 0;
        size= m_mesh.vertex_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, m_mesh.vertex_buffer());
        // et configure l'attribut 0, vec3 position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset);
        glEnableVertexAttribArray(0);
        
        // transfere les texcoords des sommets
        offset= offset + size;
        size= m_mesh.texcoord_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, m_mesh.texcoord_buffer());
        // et configure l'attribut 1, vec2 texcoord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset);
        glEnableVertexAttribArray(1);
        
        // transfere les normales des sommets
        offset= offset + size;
        size= m_mesh.normal_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, m_mesh.normal_buffer());
        // et configure l'attribut 2, vec3 normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset);
        glEnableVertexAttribArray(2);
        
        // index buffer
        glGenBuffers(1, &m_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh.index_buffer_size(), m_mesh.index_buffer(), GL_STATIC_DRAW);
        
        // nettoyage
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        // etape 1 : creer le shader program
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
        glDeleteBuffers(1, &m_vertex_buffer);
        glDeleteBuffers(1, &m_index_buffer);
        glDeleteVertexArrays(1, &m_vao);
        release_program(m_program);
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        static bool wireframe= false;
        if(key_state('w'))
        {
            clear_key_state('w');
            wireframe= !wireframe;
        }
        
        if(!wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
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
        Transform model; //= RotationX(global_time() / 20);
        Transform view= m_camera.view();
        Transform projection= m_camera.projection(window_width(), window_height(), 45);
        
        // . composer les transformations : model, view et projection
        Transform mv= view * model;
        Transform mvp= projection * mv;
         
        // . parametrer le shader program :
        //   . transformation : la matrice declaree dans le vertex shader s'appelle mvpMatrix
        program_uniform(m_program, "mvpMatrix", mvp);
        program_uniform(m_program, "mvMatrix", mv);
        
        // . parametres "supplementaires" :
        //   . couleur des pixels, cf la declaration 'uniform vec4 color;' dans le fragment shader
        program_uniform(m_program, "mesh_color", vec4(1, 1, 0, 1));
        
        static bool flat= false;
        if(key_state('f'))
        {
            clear_key_state('f');
            flat= !flat;
        }
        
        // go !
        glBindVertexArray(m_vao);
    #if 1
        for(int i= 0; i < (int) m_mesh.material_groups.size(); i++)
        {
            program_uniform(m_program, "mesh_color", Color((i % 100) / 99.f, 1 - (i % 10) / 9.f, (i % 4) / 3.f));
            
            glDrawElements(GL_TRIANGLES, m_mesh.material_groups[i].count, 
                GL_UNSIGNED_INT, m_mesh.index_buffer_offset(m_mesh.material_groups[i].first));
        }
    #else
        glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);
    #endif
        
        return 1;
    }

protected:
    MeshBuffer m_mesh;
    GLuint m_vao;
    GLuint m_vertex_buffer;
    GLuint m_index_buffer;
    int m_vertex_count;
    int m_index_count;

    Transform m_model;
    Orbiter m_camera;
    GLuint m_texture;
    GLuint m_program;

    const char *m_filename;
};


int main( int argc, char **argv )
{
    const char *filename= "data/bigguy.obj";
    if(argc > 1)
        filename= argv[1];
    
    MeshViewer tp(filename);
    tp.run();
    
    return 0;
}
