//! \file mesh_viewer.cpp

#include <algorithm>

#include "mat.h"
#include "mesh_data.h"
#include "mesh_buffer.h"
#include "material_data.h"

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
        
        // calcule l'englobant 
        Point pmin, pmax;
        bounds(data, pmin, pmax);
        m_camera.lookat(pmin, pmax);
        
        // recalcule les normales des sommets, si necessaire
        if(data.normals.size() == 0)
        {
            normals(data);
            
            printf("normals : %d positions, %d texcoords, %d normals, %d triangles\n", 
                (int) data.positions.size(), (int) data.texcoords.size(), (int) data.normals.size(), (int) data.material_indices.size());
        }
        
        // construit les buffers
        m_mesh= buffers(data);
        
        // conserve le nombre de sommets et d'indices
        m_vertex_count= m_mesh.positions.size();
        m_index_count= m_mesh.indices.size();
        
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
    
        // charge les textures des matieres
        read_textures(m_mesh.materials);
        
        // configure le filtrage des textures de l'unite 0
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f);

        // configure le filtrage des textures, mode repeat
        glGenSamplers(1, &m_sampler);
        glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        // creer le shader program
        m_program= read_program("tutos/mesh_viewer.glsl");
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
        release_textures(m_mesh.materials);
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
        else if(mb & SDL_BUTTON(2))         // le bouton milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
            //~ m_camera.move(mx);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());

        SDL_MouseWheelEvent wheel= wheel_event();
        if(wheel.y != 0)
        {
            clear_wheel_event();
            m_camera.move(16.f * wheel.y);
        }

        
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
        program_uniform(m_program, "diffuse_color", vec4(1, 1, 0, 1));
        
        static bool flat= false;
        if(key_state('f'))
        {
            clear_key_state('f');
            flat= !flat;
        }
        
        // go !
        glBindVertexArray(m_vao);
        for(int i= 0; i < (int) m_mesh.material_groups.size(); i++)
        {
            //~ program_uniform(m_program, "color", Color((i % 100) / 99.f, 1 - (i % 10) / 9.f, (i % 4) / 3.f));

            const MaterialData& material= m_mesh.materials[m_mesh.material_groups[i].material];
            
            // parametre le shader program avec la description de la matiere
            
        #if 0
            // sans utiliser de texture, recupere la couleur de la matiere et la couleur moyenne de la texture
            program_uniform(m_program, "diffuse_color", material.diffuse * material.diffuse_texture_color);
            
        #else
            // OU : couleur de base * texture
            program_uniform(m_program, "diffuse_color", material.diffuse);
            
            // utilise une texture 
            // . selectionne l'unite de texture 0
            glActiveTexture(GL_TEXTURE0);
            // . selectionne la texture 
            glBindTexture(GL_TEXTURE_2D, material.diffuse_texture);
            // . parametre le shader avec le numero de l'unite sur laquelle est selectionee la texture
            GLint location= glGetUniformLocation(m_program, "diffuse_texture");
            glUniform1i(location, 0);
            
            // . parametres de filtrage
            glBindSampler(0, m_sampler);
            
            // ou 
            // #include "uniforms.h"
            // program_use_texture(m_program, "diffuse_texture", 0, material.diffuse_texture, m_sampler);
        #endif
        
            glDrawElements(GL_TRIANGLES, m_mesh.material_groups[i].count, 
                GL_UNSIGNED_INT, m_mesh.index_buffer_offset(m_mesh.material_groups[i].first));
        }
        
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
    GLuint m_sampler;
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
