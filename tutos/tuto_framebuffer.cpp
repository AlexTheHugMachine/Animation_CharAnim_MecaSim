
//! \file tuto_framebuffer.cpp utilisation de framebuffers, fbo.

#include "vec.h"
#include "mat.h"

#include "mesh.h"
#include "wavefront.h"
#include "texture.h"

#include "program.h"
#include "uniforms.h"

#include "orbiter.h"
#include "app.h"        // classe Application a deriver


class Framebuffer : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    Framebuffer( ) : App(1024, 640) {}
    
    int init( )
    {
        m_framebuffer_width= 512;
        m_framebuffer_height= 512;
        
        // etape 1 : creer une texture couleur...
        glGenTextures(1, &m_color_buffer);
        glBindTexture(GL_TEXTURE_2D, m_color_buffer);
        
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGBA, m_framebuffer_width, m_framebuffer_height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        // ... et tous ses mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // et son sampler
        glGenSamplers(1, &color_sampler);
        
        glSamplerParameteri(color_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(color_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(color_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(color_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        
        // etape 1 : creer aussi une texture depth, sinon pas de zbuffer...
        glGenTextures(1, &m_depth_buffer);
        glBindTexture(GL_TEXTURE_2D, m_depth_buffer);
        
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_DEPTH_COMPONENT, m_framebuffer_width, m_framebuffer_height, 0,
            GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
        
        // etape 2 : creer et configurer un framebuffer object
        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER,  /* attachment */ GL_COLOR_ATTACHMENT0, /* texture */ m_color_buffer, /* mipmap level */ 0);
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER,  /* attachment */ GL_DEPTH_ATTACHMENT, /* texture */ m_depth_buffer, /* mipmap level */ 0);
        
        // le fragment shader ne declare qu'une seule sortie, indice 0
        GLenum buffers[]= { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, buffers);
        
        // nettoyage
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        
        // charge un mesh
        Mesh mesh= read_mesh("data/cube.obj");
        if(mesh == Mesh::error()) return -1;
        
        Point pmin, pmax;
        mesh.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);
        m_framebuffer_camera.lookat(pmin, pmax);
        
        m_vertex_count= mesh.vertex_count();

        m_color_texture= read_texture(0, "data/debug2x2red.png");
        
        // cree les buffers et le vao
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        // buffer : positions + texcoords + normals
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        
        size_t size= mesh.vertex_buffer_size() + mesh.texcoord_buffer_size() + mesh.normal_buffer_size();
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
        
        // transfere les positions des sommets
        size_t offset= 0;
        size= mesh.vertex_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, mesh.vertex_buffer());
        // configure l'attribut 0, vec3 position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset);
        glEnableVertexAttribArray(0);

        // transfere les texcoords des sommets
        offset= offset + size;
        size= mesh.texcoord_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, mesh.texcoord_buffer());
        // configure l'attribut 1, vec2 texcoord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset);
        glEnableVertexAttribArray(1);

        // transfere les normales des sommets
        offset= offset + size;
        size= mesh.normal_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, mesh.normal_buffer());
        // configure l'attribut 2, vec3 normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, (const GLvoid *) offset);
        glEnableVertexAttribArray(2);
        
        //
        mesh.release();
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // shaders
        m_texture_program= read_program("tutos/texcoords.glsl");
        program_print_errors(m_texture_program);
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepthf(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest

        return 0;   // ras, pas d'erreur
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        glDeleteTextures(1, &m_color_buffer);
        glDeleteTextures(1, &m_depth_buffer);
        glDeleteFramebuffers(1, &m_framebuffer);
        glDeleteSamplers(1, &color_sampler);
        
        release_program(m_texture_program);
        glDeleteTextures(1, &m_color_texture);
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_buffer);
        return 0;
    }
    
    int update( const float time, const float delta )
    {
        m_model= RotationY(time / 20);
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        // deplace la camera
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.move(mx);
        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());

        {
            /* passe 1 : dessiner dans le framebuffer
             */
            
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
            glViewport(0, 0, m_framebuffer_width, m_framebuffer_height);
            glClearColor(1, 1, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // draw "classique"
            glBindVertexArray(m_vao);
            glUseProgram(m_texture_program);
            
            Transform m= m_model;
            Transform v= m_framebuffer_camera.view();
            Transform p= m_framebuffer_camera.projection(m_framebuffer_width, m_framebuffer_height, 45);
            Transform mvp= p * v * m;
            Transform mv= v * m;
            
            program_uniform(m_texture_program, "mvpMatrix", mvp);
            program_uniform(m_texture_program, "mvMatrix", mv);
            program_uniform(m_texture_program, "normalMatrix", mv.normal());
            program_use_texture(m_texture_program, "color_texture", 0, m_color_texture, 0);
            
            glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
        }
        
        if(key_state(' '))
        {
            /* montrer le resultat de la passe 1
                copie le framebuffer sur la fenetre
             */
            
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glViewport(0, 0, window_width(), window_height());
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            
            glBlitFramebuffer(
                0, 0, m_framebuffer_width, m_framebuffer_height,        // rectangle origine dans READ_FRAMEBUFFER
                0, 0, m_framebuffer_width, m_framebuffer_height,        // rectangle destination dans DRAW_FRAMEBUFFER
                GL_COLOR_BUFFER_BIT, GL_LINEAR);                        // ne copier que la couleur (+ interpoler)
        }
        else
        {
            /* passe 2 : utiliser la texture du framebuffer 
             */
            
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glViewport(0, 0, window_width(), window_height());
            glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // draw "classique"
            glBindVertexArray(m_vao);
            glUseProgram(m_texture_program);
            
            Transform m= m_model;
            Transform v= m_camera.view();
            Transform p= m_camera.projection(window_width(), window_height(), 45);
            Transform mvp= p * v * m;
            Transform mv= v * m;
            
            program_uniform(m_texture_program, "mvpMatrix", mvp);
            program_uniform(m_texture_program, "mvMatrix", mv);
            program_uniform(m_texture_program, "normalMatrix", mv.normal());
            
            // utilise la texture attachee au framebuffer
            program_uniform(m_texture_program, "color_texture", 0);     // utilise la texture configuree sur l'unite 0
            
            // configure l'unite 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_color_buffer);
            glBindSampler(0, color_sampler);
            
            // SOIT :
            // bloque le filtrage de la texture pour n'utiliser que le mipmap 0
            //~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
            
            // OU :
            // recalcule les mipmaps de la texture... ils sont necessaires pour afficher le cube texture, 
            // pourquoi ? un draw dans un framebuffer ne modifie que le mipmap 0, pas les autres, donc il faut les recalculer...
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, miplevels(m_framebuffer_width, m_framebuffer_height));
            glGenerateMipmap(GL_TEXTURE_2D);
            
            // go
            glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
        }
        
        return 1;
    }

protected:
    Transform m_model;
    Orbiter m_camera;
    Orbiter m_framebuffer_camera;

    GLuint m_vao;
    GLuint m_buffer;
    GLuint m_texture_program;
    int m_vertex_count;

    GLuint m_color_texture;
    GLuint color_sampler;

    GLuint m_color_buffer;
    GLuint m_depth_buffer;
    GLuint m_framebuffer;
    int m_framebuffer_width;
    int m_framebuffer_height;
};


int main( int argc, char **argv )
{
    Framebuffer tp;
    tp.run();
    
    return 0;
}
    
