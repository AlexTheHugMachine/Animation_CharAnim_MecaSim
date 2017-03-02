
//! \file tuto_storage_texture.cpp  exemple direct d'utilisation d'une storage texture / image.  le fragment shader compte combien de fragments sont calcules par pixel.

#include "app_time.h"

#include "vec.h"
#include "mat.h"

#include "program.h"
#include "uniforms.h"
#include "texture.h"

#include "mesh.h"
#include "wavefront.h"

#include "orbiter.h"


class StorageImage : public AppTime
{
public:
    // application openGL 4.3
    StorageImage( ) : AppTime(1024, 640,  4, 3) {}
    
    int init( )
    {
        m_mesh= read_mesh("data/bigguy.obj");
        
        Point pmin, pmax;
        m_mesh.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);
        
        // cree la texture, 1 canal, entiers 32bits non signes
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_R32UI, window_width(), window_height(), 0,
            GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);  // GL_RED_INTEGER, sinon normalisation implicite...
        
        // pas la peine de construire les mipmaps / pas possible pour une texture int / uint
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        
        // 
        m_program= read_program("tutos/tuto_storage_texture.glsl");
        program_print_errors(m_program);
        
        m_program_display= read_program("tutos/storage_texture_display.glsl");
        program_print_errors(m_program_display);
        
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
        m_mesh.release();
        release_program(m_program);
        glDeleteTextures(1, &m_texture);
        return 0;
    }
    
    int update( const float time, const float delta )
    {

        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // effacer la texture image / compteur, le shader ajoute 1 a chaque fragment dessine...
        GLuint zero= 0;
        glClearTexImage(m_texture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);
        
        // deplace la camera
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.move(mx);
        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        
        // passe 1 : compter le nombre de fragments par pixel
        glUseProgram(m_program);
        
        Transform model= RotationY(global_time() / 20);
        Transform view= m_camera.view();
        Transform projection= m_camera.projection(window_width(), window_height(), 45);
        Transform mv= view * model;
        Transform mvp= projection * mv;
        
        program_uniform(m_program, "mvpMatrix", mvp);
        
        // selectionne la texture sur l'unite image 0, operations atomiques / lecture + ecriture
        glBindImageTexture(0, m_texture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);
        program_uniform(m_program, "image", 0);
        
        m_mesh.draw(m_program);
        
        if(key_state(' ') == 0)
        {
            // passe 2 : afficher le compteur
            glUseProgram(m_program_display);

            // attendre que les resultats de la passe 1 soit disponible
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            
            // RE-selectionne la texture sur l'unite image 0 / LECTURE SEULE
            glBindImageTexture(0, m_texture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);
            program_uniform(m_program_display, "image", 0);
            
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        
        return 1;
    }

protected:
    Mesh m_mesh;
    Orbiter m_camera;

    GLuint m_program;
    GLuint m_program_display;
    GLuint m_texture;
};


int main( int argc, char **argv )
{
    StorageImage app;
    app.run();
    
    return 0;
}
