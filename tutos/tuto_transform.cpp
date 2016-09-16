
//! \file tuto_transform.cpp visualisation du volume visible par une camera, + visualisation d'un objet (par rapport au volume visible)

#include "mat.h"
#include "wavefront.h"

#include "orbiter.h"
#include "draw.h"
#include "app.h"


// renvoie vrai si la boite englobante est au moins partiellement visible
bool visible( const Transform& mvp, const Point& pmin, const Point& pmax )
{
    int planes[6] = { };
    
    // enumere les 8 sommets de la boite englobante
    for(unsigned int i= 0; i < 8; i++)
    {
        Point p= pmin;
        if(i & 1) p.x= pmax.x;
        if(i & 2) p.y= pmax.y;
        if(i & 4) p.z= pmax.z;
        
        // transformation du point homogene (x, y, z, w= 1)
        vec4 h= mvp(vec4(p));
        
        // teste la position du point homogene par rapport aux 6 faces de la region visible
        if(h.x < -h.w) planes[0]++;     // trop a gauche
        if(h.x > h.w) planes[1]++;      // trop a droite
        
        if(h.y < -h.w) planes[2]++;     // trop bas
        if(h.y > h.w) planes[3]++;      // trop haut
        
        if(h.z < -h.w) planes[4]++;     // trop pres
        if(h.z > h.w) planes[5]++;      // trop loin
    }
    
    // verifie si tous les sommets sont du "mauvais cote" d'une seule face, planes[i] == 8
    for(unsigned int i= 0; i < 6; i++)
        if(planes[i] == 8)
            return false;       // la boite englobante n'est pas visible
    
    // l'objet doit etre visible, ou pas, il faut aussi le test dans l'autre sens... 
    return true;
}


class TP : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : App(1024, 640) {}
    
    int init( )
    {
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
        
        // charge un objet a afficher
        m_objet= read_mesh("data/bigguy.obj");

        // conserve (les extremites de) sa boite englobante
        m_objet.bounds(m_pmin, m_pmax);
        m_camera.lookat(m_pmin, m_pmax);
        //~ m_camera.lookat(Point(), 10);
        
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
        
        if(key_state(' '))
        {
            // deplace la camera "normale"
            if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
                m_camera.rotation((float) mx, (float) my);
            else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
                m_camera.move((float) mx);
            else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
                m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        }
        else
        {
            // deplace l'observateur
            if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
                m_observer.rotation((float) mx / 10, (float) my / 10);
            else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
                m_observer.move((float) mx / 10);
            else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
                m_observer.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        }
        
        // affiche l'objet en rouge, si sa boite englobante n'est pas visible pour la camera.
        if(visible(m_camera.projection((float) window_width(), (float)window_height(), 45) * m_camera.view() * m_model, m_pmin, m_pmax))
            m_objet.default_color(Green());
        else
            m_objet.default_color(Red());
        
        
        Transform view= m_observer.view();
        Transform projection= Perspective(45, (float) window_width() / (float) window_height(), .1f, 1000.f);
        
        static bool wireframe= false;
        if(key_state(' '))
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            // afficher le point de vue "normal"
            draw(m_grid, m_camera);
            draw(m_objet, m_model, m_camera);
        }
        else
        {
            if(key_state('w'))
            {
                clear_key_state('w');
                wireframe= !wireframe;
            }
            
            if(!wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
        #if 1
            // afficher le volume visible de la camera dans le repere monde
            draw(m_grid, Identity(), view, projection);
            draw(m_frustum, Inverse(m_camera.projection((float) window_width(), (float) window_height(), 45) * m_camera.view()), view, projection);
            draw(m_objet, m_model, view, projection);
            
        #else
            // afficher dans le repere image 
            draw(m_grid, m_camera.projection(window_width(), window_height(), 45) * m_camera.view(), view, projection);
            draw(m_frustum, m_camera.projection(window_width(), window_height(), 45) * m_camera.view() * Inverse(m_camera.projection(window_width(), window_height(), 45) * m_camera.view()), view, projection);
            // remarque : ca se simplifie non ??
            draw(m_objet, m_camera.projection(window_width(), window_height(), 45) * m_camera.view() * m_model, view, projection);
        #endif
        }
        
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

    Point m_pmin, m_pmax;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    
    return 0;
}
