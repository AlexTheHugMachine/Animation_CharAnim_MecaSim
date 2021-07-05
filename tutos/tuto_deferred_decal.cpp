
//! \file tuto_deferred_decal.cpp


#include "wavefront.h"
#include "texture.h"
#include "program.h"
#include "uniforms.h"
#include "framebuffer.h"

#include "orbiter.h"
#include "draw.h"
#include "app_camera.h"        // classe Application a deriver


// utilitaire. creation d'une grille / repere.
// n= 0 ne dessine que les axes du repere.
Mesh make_grid( const int n= 10 )
{
    glLineWidth(2);
    Mesh grid= Mesh(GL_LINES);
    
    // grille
    grid.color(White());
    for(int x= 0; x < n; x++)
    {
        float px= float(x) - float(n)/2 + .5f;
        grid.vertex(px, 0, - float(n)/2 + .5f); 
        grid.vertex(px, 0, float(n)/2 - .5f);
    }

    for(int z= 0; z < n; z++)
    {
        float pz= float(z) - float(n)/2 + .5f;
        grid.vertex(- float(n)/2 + .5f, 0, pz); 
        grid.vertex(float(n)/2 - .5f, 0, pz); 
    }
    
    // axes XYZ
    grid.color(Red());
    grid.vertex(0, .1, 0);
    grid.vertex(1, .1, 0);
    
    grid.color(Green());
    grid.vertex(0, .1, 0);
    grid.vertex(0, 1, 0);
    
    grid.color(Blue());
    grid.vertex(0, .1, 0);
    grid.vertex(0, .1, 1);
    
    return grid;
}

// utilitaire. creation d'un plan / sol 
Mesh make_ground( const int n= 10 )
{
    Mesh grid= Mesh(GL_TRIANGLES);
    
    grid.normal(0, 1, 0);
    int a= grid.vertex(-n/2, -1, n/2);
    int b= grid.vertex( n/2, -1, n/2);
    int c= grid.vertex( n/2, -1, -n/2);
    int d= grid.vertex(-n/2, -1, -n/2);
    grid.triangle(a, b, c);
    grid.triangle(a, c, d);
    
    return grid;
}

Mesh make_xyz( )
{
    glLineWidth(2);
    Mesh camera= Mesh(GL_LINES);
    
    // axes XYZ
    camera.color(Red());
    camera.vertex(Point(0, 0, 0));
    camera.vertex(Point(1, 0, 0));
    
    camera.color(Green());
    camera.vertex(Point(0, 0, 0));
    camera.vertex(Point(0, 1, 0));
    
    camera.color(Blue());
    camera.vertex(Point(0, 0, 0));
    camera.vertex(Point(0, 0, 1));
    
    return camera;
}

Mesh make_frustum( )
{
    glLineWidth(2);    
    Mesh camera= Mesh(GL_LINES);
    
    camera.color(Yellow());
    // face avant
    camera.vertex(-1, -1, -1);
    camera.vertex(-1, 1, -1);
    camera.vertex(-1, 1, -1);
    camera.vertex(1, 1, -1);
    camera.vertex(1, 1, -1);
    camera.vertex(1, -1, -1);
    camera.vertex(1, -1, -1);
    camera.vertex(-1, -1, -1);
    
    // face arriere
    camera.vertex(-1, -1, 1);
    camera.vertex(-1, 1, 1);
    camera.vertex(-1, 1, 1);
    camera.vertex(1, 1, 1);
    camera.vertex(1, 1, 1);
    camera.vertex(1, -1, 1);
    camera.vertex(1, -1, 1);
    camera.vertex(-1, -1, 1);
    
    // aretes
    camera.vertex(-1, -1, -1);
    camera.vertex(-1, -1, 1);
    camera.vertex(-1, 1, -1);
    camera.vertex(-1, 1, 1);
    camera.vertex(1, 1, -1);
    camera.vertex(1, 1, 1);
    camera.vertex(1, -1, -1);
    camera.vertex(1, -1, 1);
    
    return camera;
}

class TP : public AppCamera
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : AppCamera(1024, 640) {}
    
    // creation des objets de l'application
    int init( )
    {
        // decrire un repere / grille 
        m_repere= make_grid(20);
        m_local= make_grid(2);
        //~ m_ground= make_ground(20);
        m_ground= read_mesh("ground.obj");
        m_proxy= make_xyz();
        m_frustum= make_frustum();
        m_frustum_cube= read_mesh("data/frustum.obj");
        
        // charge l'element
        m_objet= read_mesh("data/robot.obj");
        
        //~ m_texture= read_texture(0, "data/grid.png");
        //~ m_texture= read_texture(0, "decal_shadow.png");
        m_texture= read_texture(0, "orange_splash.png");
        //~ m_texture= read_texture(0, "yellow_splash.png");
        //~ m_texture= read_texture(0, "red_splash.png");
        
        m_mesh_program= read_program("tutos/deferred_mesh.glsl");
        program_print_errors(m_mesh_program);
        m_decal_program= read_program("tutos/deferred_decal.glsl");
        program_print_errors(m_decal_program);
        
        // position initiale de l'objet
        m_position= Identity();
        
        // si l'objet est "gros", il faut regler la camera pour l'observer entierement :
        // recuperer les points extremes de l'objet (son englobant)
        Point pmin, pmax;
        m_ground.bounds(pmin, pmax);
        // parametrer la camera de l'application, renvoyee par la fonction camera()
        camera().lookat(pmin, pmax);
        
        m_framebuffer.create(window_width(), window_height());
        m_framebuffer.clear_color(Black());
        m_framebuffer.clear_depth(1);
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LEQUAL);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest
        
        return 0;   // ras, pas d'erreur
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        m_objet.release();
        m_repere.release();
        m_local.release();
        m_ground.release();
        m_proxy.release();
        
        m_framebuffer.release();
        release_program(m_mesh_program);
        release_program(m_decal_program);
        glDeleteTextures(1, &m_texture);
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // modifie la position de l'objet en fonction des fleches de direction
        if(key_state(SDLK_UP))
            m_position= m_position * Translation(0, 0, 0.15);     // en avant
        if(key_state(SDLK_DOWN))
            m_position= m_position * Translation(0, 0, -0.15);      // en arriere
        if(key_state(SDLK_PAGEUP))
            m_position= m_position * Translation(0, 0.15, 0);     // en haut
        if(key_state(SDLK_PAGEDOWN))
            m_position= m_position * Translation(0, -0.15, 0);      // en bas
            
        if(key_state(SDLK_LEFT))
            m_position= m_position * RotationY(2);     // tourne vers la droite
        if(key_state(SDLK_RIGHT))
            m_position= m_position * RotationY(-2);     // tourne vers la gauche
        
        
        // transformations de la camera de l'application
        Transform view= camera().view();
        Transform projection= camera().projection();
        
    // dessine les objets dans le framebuffer... ceux sur lesquels on veut projetter le decal... 
        {
            m_framebuffer.bind(0, /* store_color */ true, /* store_depth */ true, /* store_position */ false, /* store_texcoord */ false, /* store_normal */ false, /* store_material */ false);
            // dessine le decor
            draw(m_ground, /* model */ Identity(), view, projection);
            
            //~ // dessine l'objet, ou pas ...
            //~ draw(m_objet, /* model */ m_position, view, projection);
            
            m_framebuffer.unbind(window_width(), window_height());
        }
        
    // copie les textures vers la fenetre
        m_framebuffer.blit_color(0, 0, window_width(), window_height());
        m_framebuffer.blit_depth(0, 0, window_width(), window_height());
        
    // positionne le decal
        Transform r= RotationX(-90);
        Transform t= Translation(0,0, 8);
        Transform m= r * t;
        
        Transform decal_view= Inverse(m_position * m);
        //~ Transform decal_projection= Perspective(35, 1, float(0.1), float(10));
        Transform decal_projection= Ortho(-2, 2, -2, 2, float(0.1), float(10));
        
        // dessine le frustum pour verifier que le placement est correct...
        {
            // passage repere projection vers global : inverse de projection*view
            Transform decal_model= Inverse(decal_projection * decal_view);
            
            // juste les aretes 
            draw(m_frustum, decal_model, view, projection);
            //~ // ou le proxy / un cube 
            //~ draw(m_frustum_cube, decal_model, view, projection);
        
            // dessine le repere utilise pour placer le decal...
            draw(m_proxy, /* model */ m_position * m, view, projection);
        }
        
    // dessine / projette le decal sur les objets dessines dans le framebuffer et modifie la couleur des pixels de la fenetre.
    // == execute le fragment shader pour les pixels sur lesquels le decal peut se projetter
        glUseProgram(m_decal_program);
        {
            // passage repere projection vers global : inverse de projection*view
            {
                Transform model= Inverse(decal_projection * decal_view);
                Transform mv= view * model;
                Transform mvp= projection * mv;
                
                program_uniform(m_decal_program, "mvpMatrix", mvp);
            }
            
            {
                // reprojette le fragment stocke dans le framebuffer dans le repere du monde
                Transform viewport= Viewport(window_width(), window_height());
                Transform m= Inverse(viewport * projection * view);
                // puis passage du monde dans le repere de projection du decal
                Transform decal_viewport= Viewport(1, 1);
                Transform inv= decal_viewport * decal_projection * decal_view * m;
                
                program_uniform(m_decal_program, "invMatrix", inv);
            }
            
            program_use_texture(m_decal_program, "decal_texture", 0, m_texture);
            m_framebuffer.use_color_texture(m_decal_program, "color_texture", 1);
            m_framebuffer.use_depth_texture(m_decal_program, "depth_texture", 2);
            
            // dessiner sans modifier le zbuffer / la profondeur stockee
            glDepthMask(GL_FALSE);
            m_frustum_cube.draw(m_decal_program, /* position */ true, /* texcoord */ false, /* normal */ false, /* color */ false, /* material id */ false);
            
            glDepthMask(GL_TRUE);
        }
        
    // dessine les objets, sans decals...
        // dessine l'objet, ou pas ...
        draw(m_objet, /* model */ m_position, view, projection);
        
        // dessine aussi le repere local
        draw(m_local, /* model */ m_position, view, projection);
        
        // screenshot
        if(key_state('s'))
        {
            clear_key_state('s');
            static int id= 1;
            screenshot("camera", id++);
        }
        
        // continuer...
        return 1;
    }

protected:
    Mesh m_objet;
    Mesh m_repere;
    Mesh m_local;
    Mesh m_proxy;
    Mesh m_frustum;
    Mesh m_frustum_cube;
    Mesh m_ground;
    Transform m_position;
    Framebuffer m_framebuffer;
    GLuint m_texture;
    GLuint m_mesh_program;
    GLuint m_decal_program;
};


int main( int argc, char **argv )
{
    // il ne reste plus qu'a creer un objet application et la lancer 
    TP tp;
    tp.run();
    
    return 0;
}
