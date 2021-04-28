
//! \file tuto_transformations.cpp


#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "draw.h"        
#include "app_camera.h"        // classe Application a deriver


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
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : AppCamera(1024, 640) {}
    
    // creation des objets de l'application
    int init( )
    {
        // decrire un repere / grille 
        m_repere= make_grid(20);
        m_local= make_grid(4);
        
        // charge l'element
        m_objet= read_mesh("data/cube.obj");
        
        // position initiale de l'objet
        m_position= Translation(0, 0.5, 0);
        
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
        
        draw(m_objet, /* model */ m_position, camera());
      
    // dessine le meme objet a un autre endroit. il faut modifier la matrice model, qui sert a ca : placer un objet dans le monde, ailleurs qu'a l'origine.
        // par exemple, a la verticale au dessus du premier cube :
        // la transformation est une translation le long du vecteur Y= (0, 1, 0), si on veut placer le cube plus haut, il suffit d'utiliser une valeur > 1
        Transform t= Translation(0, 2, 0);
        
        // et si on veut le faire tourner en fonction du temps ?
        float time= global_time();
        Transform r= RotationX(time / float(20));
        
        // et pour placer et orienter, les 2 a la fois ? on compose les 2 transformations, et il suffit de multiplier les 2 matrices
        // mais attention : l'ordre de la multiplication des matrices change le resultat...
        //~ // solution 1 : tourner le cube puis le deplacer
        //~ Transform m= t * r;
        
        // solution 2 : deplacer puis tourner le cube
        Transform m= r * t;
        
    // dessine le 2ieme objet par rapport au premier objet... 
    // compose leurs 2 transformations, les coordonnees du 2ieme objet sont connues dans le repere local du premier objet
        draw(m_objet, /* model */ m_position * m, camera());
        
        // dessine aussi le repere local, pour le meme point de vue
        draw(m_local, m_position, camera());
        
        // dessine le repere global, pour le meme point de vue
        draw(m_repere, Identity(), camera());
        
        // continuer...
        return 1;
    }

protected:
    Mesh m_objet;
    Mesh m_repere;
    Mesh m_local;
    Transform m_position;
};


int main( int argc, char **argv )
{
    // il ne reste plus qu'a creer un objet application et la lancer 
    TP tp;
    tp.run();
    
    return 0;
}
