
//! \file tuto_transformations_camera.cpp


#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "draw.h"        
#include "app_camera.h"        // classe Application a deriver


// utilitaire. creation d'une grille / repere.
// n= 0 ne dessine que les axes du repere.
Mesh make_grid( const int n= 10 )
{
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
    
    glLineWidth(2);
    
    return grid;
}

Mesh make_camera( )
{
    Mesh camera= Mesh(GL_LINES);
    
    camera.color(Yellow());
    camera.vertex(0,0,0);
    camera.vertex(-0.5, -0.5, -1);
    camera.vertex(0,0,0);
    camera.vertex(-0.5, 0.5, -1);
    camera.vertex(0,0,0);
    camera.vertex(0.5, 0.5, -1);
    camera.vertex(0,0,0);
    camera.vertex(0.5, -0.5, -1);
    
    camera.vertex(-0.5, -0.5, -1);
    camera.vertex(-0.5, 0.5, -1);

    camera.vertex(-0.5, 0.5, -1);
    camera.vertex(0.5, 0.5, -1);

    camera.vertex(0.5, 0.5, -1);
    camera.vertex(0.5, -0.5, -1);
    
    camera.vertex(0.5, -0.5, -1);
    camera.vertex(-0.5, -0.5, -1);
    
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
    
    glLineWidth(2);
    
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
        m_proxy= make_camera();
        
        // charge l'element
        m_objet= read_mesh("data/cube.obj");
        
        // position initiale de l'objet
        m_position= Identity();
        
        // si l'objet est "gros", il faut regler la camera pour l'observer entierement :
        // recuperer les points extremes de l'objet (son englobant)
        Point pmin, pmax;
        m_repere.bounds(pmin, pmax);
        // parametrer la camera de l'application, renvoyee par la fonction camera()
        camera().lookat(pmin, pmax);
        
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
        m_objet.release();
        m_repere.release();
        m_local.release();
        m_proxy.release();
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // modifie la position de l'objet en fonction des fleches de direction
        if(key_state(SDLK_UP))
            m_position= m_position * Translation(0, 0, 0.25);     // en avant
        if(key_state(SDLK_DOWN))
            m_position= m_position * Translation(0, 0, -0.25);      // en arriere
            
        if(key_state(SDLK_LEFT))
            m_position= m_position * RotationY(4);     // tourne vers la droite
        if(key_state(SDLK_RIGHT))
            m_position= m_position * RotationY(-4);     // tourne vers la gauche
        
      
    // dessine le meme objet a un autre endroit. il faut modifier la matrice model, qui sert a ca : placer un objet dans le monde, ailleurs qu'a l'origine.
        // par exemple, a la verticale au dessus du premier cube :
        // la transformation est une translation le long du vecteur Y= (0, 1, 0), si on veut placer le cube plus haut, il suffit d'utiliser une valeur > 1
        Transform t= Translation(0, 2, 0);
        
        float time= global_time();
        static Transform r;
        if(!key_state('f'))
            // rotation en fonction du temps...
            r= RotationX(time / float(20));
        else
        {
            // place la camera derriere le cube
            r= RotationX(15) * RotationY(180);
            t= Translation(1, 1, 4);
        }
        // et pour placer et orienter, les 2 a la fois ? on compose les 2 transformations, et il suffit de multiplier les 2 matrices
        // mais attention : l'ordre de la multiplication des matrices change le resultat...
        Transform m= r * t;
        
        // transformations de la camera de l'application
        Transform view= camera().view();
        Transform projection= camera().projection();
        
        if(key_state(' '))
        {
            view= Inverse(m_position * m);
            projection= Perspective(60, float(window_width()) / float(window_height()), float(0.1), float(40));
        }
        
        draw(m_objet, /* model */ m_position, view, projection);
        
    // dessine le 2ieme objet par rapport au premier objet... 
    // compose leurs 2 transformations, les coordonnees du 2ieme objet sont connues dans le repere local du premier objet
        draw(m_proxy, /* model */ m_position * m, view, projection);
        
        // dessine aussi le repere local, pour le meme point de vue
        draw(m_local, m_position, view, projection);
        
        // dessine le repere global, pour le meme point de vue
        draw(m_repere, Identity(), view, projection);
        
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
    Transform m_position;
};


int main( int argc, char **argv )
{
    printf("appuyez sur 'espace' pour fixer la position de la camera\n");
    printf("appuyez sur 'c' pour dessiner avec la camera\n");
    printf("appuyez sur 'c' et 'espace' pour dessiner avec la camera attachee derriere le cube\n");
    printf("deplacez le cube avec les fleches de directions\n");

    // il ne reste plus qu'a creer un objet application et la lancer 
    TP tp;
    tp.run();
    
    return 0;
}
