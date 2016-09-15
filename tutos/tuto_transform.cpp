
//! \file tuto_transform.cpp visualisation du volume visible par une camera, + visualisation d'un triangle (par rapport au volume visible)

#include "mat.h"
#include "wavefront.h"

#include "orbiter.h"
#include "draw.h"
#include "widgets.h"
#include "app.h"


class TP : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : App(1024, 640) {}
    
    int init( )
    {
        m_widgets= create_widgets();
        
        // volume visible par une camera, un cube -1 1
        m_frustum= read_mesh("data/frustum.obj");
        
        // grille / plan de reference
        m_grid.create(GL_LINES);
        for(int x= 0; x < 10; x++)
        {
            float px= (float) x - 5.f + 0.5f;
            m_grid.vertex(px, 0, -4.5f); 
            m_grid.vertex(px, 0, 4.5f); 
        }

        for(int z= 0; z < 10; z++)
        {
            float pz= (float) z - 5.f + 0.5f;
            m_grid.vertex(-4.5f, 0, pz); 
            m_grid.vertex(4.5f, 0, pz); 
        }
        
        // cree un objet simple, un triangle par exemple
    #if 0
        m_a= Point(-2, -1, 0);
        m_b= Point(2, -1, 0);
        m_c= Point(0, 1, 0);
        
        m_objet.create(GL_TRIANGLES);
        m_objet.vertex(m_a);
        m_objet.vertex(m_b);
        m_objet.vertex(m_c);
    #else
        // ou un objet plus "complique"
        m_objet= read_mesh("data/bigguy.obj");
        //~ m_camera.lookat(Point(), 10);
        
        Point pmin, pmax;
        m_objet.bounds(pmin, pmax);
        m_camera.lookat(pmin, pmax);
    #endif
        
        m_objet.default_color(Green());
        
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
        release_widgets(m_widgets);
        m_frustum.release();
        m_grid.release();
        m_objet.release();
        glDeleteTextures(1, &m_texture);
        
        return 0;
    }
    
    int update( const float time, const float delta )
    {
        // modifier l'orientation du cube a chaque image. 
        // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
        // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.
        
        m_model= RotationY(time / 20);
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
            m_observer.rotation((float) mx / 10, (float) my / 10);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_observer.move((float) mx / 10);
        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            m_observer.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        
        Transform view= m_observer.view();
        Transform projection= Perspective(45, (float) window_width() / (float) window_height(), .1f, 1000.f);
        
        draw(m_grid, m_camera.view() * m_model, view, projection);
        draw(m_frustum, Inverse(m_camera.projection(window_width(), window_height(), 45) * m_camera.view()), view, projection);
        draw(m_objet, m_camera.view() * m_model, view, projection);
        
    #if 0
        static char tmpa[48]= { 0 };
        static char tmpb[48]= { 0 };
        static char tmpc[48]= { 0 };
        begin(m_widgets);
            //~ sprintf(tmpa, "%f %f %f", m_a.x, m_a.y, m_a.z);
            label(m_widgets, "a");
            if(edit(m_widgets, sizeof(tmpa), tmpa))
            {
                float x, y, z;
                if(sscanf(tmpa, "%f %f %f", &x, &y, &z) == 3)
                {
                    m_a= Point(x, y, z);
                    m_objet.vertex(0, m_a);
                    sprintf(tmpa, "%f %f %f", m_a.x, m_a.y, m_a.z);
                }
            }

            //~ sprintf(tmpb, "%f %f %f", m_b.x, m_b.y, m_b.z);
            begin_line(m_widgets);
            label(m_widgets, "b");
            if(edit(m_widgets, sizeof(tmpb), tmpb))
            {
                float x, y, z;
                if(sscanf(tmpb, "%f %f %f", &x, &y, &z) == 3)
                {
                    m_b= Point(x, y, z);
                    m_objet.vertex(1, m_b);
                    sprintf(tmpb, "%f %f %f", m_b.x, m_b.y, m_b.z);
                }
            }
            
            //~ sprintf(tmpc, "%f %f %f", m_c.x, m_c.y, m_c.z);
            begin_line(m_widgets);
            label(m_widgets, "c");
            if(edit(m_widgets, sizeof(tmpc), tmpc))
            {
                float x, y, z;
                if(sscanf(tmpc, "%f %f %f", &x, &y, &z) == 3)
                {
                    m_c= Point(x, y, z);
                    m_objet.vertex(2, m_c);
                    sprintf(tmpc, "%f %f %f", m_c.x, m_c.y, m_c.z);
                }
            }
            
        end(m_widgets);
        draw(m_widgets, window_width(), window_height());
    #endif
    
        return 1;
    }

protected:
    Transform m_model;
    Mesh m_frustum;
    Mesh m_objet;
    Mesh m_grid;
    GLuint m_texture;
    Orbiter m_camera;
    Orbiter m_observer;

    Widgets m_widgets;
    int m_mode;
    Point m_a;
    Point m_b;
    Point m_c;
};

int main( )
{
    TP tp;
    tp.run();
    
    return 0;
}
