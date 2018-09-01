
//! \file tuto9_buffers.cpp utilisation d'un shader 'utilisateur' pour afficher un objet Mesh + creation des buffers / vertex array object

#include "mat.h"
#include "mesh.h"
#include "wavefront.h"

#include "orbiter.h"
#include "program.h"
#include "uniforms.h"
#include "draw.h"

#include "app.h"        // classe Application a deriver

struct Buffers
{
    GLuint vao;
    GLuint vertex_buffer;
    int vertex_count;

    Buffers( ) : vao(0), vertex_buffer(0), vertex_count(0) {}
    
    void create( const Mesh& mesh )
    {
        if(!mesh.vertex_buffer_size()) return;
        
        // cree et initialise le buffer: conserve la positions des sommets
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_buffer_size(), mesh.vertex_buffer(), GL_STATIC_DRAW);
        
        // cree et configure le vertex array object: conserve la description des attributs de sommets
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        // attribut 0, position des sommets, declare dans le vertex shader : in vec3 position;
        glVertexAttribPointer(0, 
            3, GL_FLOAT,    // size et type, position est un vec3 dans le vertex shader
            GL_FALSE,       // pas de normalisation des valeurs
            0,              // stride 0, les valeurs sont les unes a la suite des autres
            0               // offset 0, les valeurs sont au debut du buffer
        );
        glEnableVertexAttribArray(0);
        
        // conserve le nombre de sommets
        vertex_count= mesh.vertex_count();
    }
    
    void release( )
    {
        glDeleteBuffers(1, &vertex_buffer);
        glDeleteVertexArrays(1, &vao);
    }
};


class TP : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : App(1024, 640) {}
    
    int init( )
    {
        // charger un objet a afficher
        Mesh mesh= read_mesh("data/cube.obj");
        if(!mesh.vertex_count()) return -1;
        
        // etape 1 : creer le vertex buffer et le vao
        m_objet.create(mesh);
        
        // etape 2 : configurer la camera
        Point pmin, pmax;
        mesh.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);
        
        // etape 3 : creer le shader program
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
        // etape 4 : detruire le shader program
        release_program(m_program);
        m_objet.release();
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
    
        // . recuperer les transformations
        Transform model= RotationX(global_time() / 20);
        Transform view= m_camera.view();
        Transform projection= m_camera.projection(window_width(), window_height(), 45);
        
        // . composer les transformations : model, view et projection
        Transform mvp= projection * view * model;
        
        // configurer le shader program
        int location;
        glUseProgram(m_program);
        
        //  . transformation : la matrice declaree dans le vertex shader s'appelle mvpMatrix
        location= glGetUniformLocation(m_program, "mvpMatrix");
        glUniformMatrix4fv(location, 1, GL_TRUE, mvp.buffer());
        
        // . parametres "supplementaires" :
        // . couleur des pixels, cf la declaration 'uniform vec4 color;' dans le fragment shader
        location= glGetUniformLocation(m_program, "color");
        glUniform4f(location, 1, 1, 0, 1);
        
        // go !
        // selectionner les attributs et les buffers de l'objet
        glBindVertexArray(m_objet.vao);
        
        // dessiner les triangles de l'objet
        glDrawArrays(GL_TRIANGLES, 0, m_objet.vertex_count);
        
        return 1;
    }

protected:
    Transform m_model;
    Buffers m_objet;
    Orbiter m_camera;
    GLuint m_texture;
    GLuint m_program;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    
    return 0;
}
