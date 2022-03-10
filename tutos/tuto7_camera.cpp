
//! \file tuto7_camera.cpp reprise de tuto7.cpp mais en derivant AppCamera, avec gestion automatique d'une camera.


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
        m_repere= make_grid(10);
        
        // charge un objet
        m_cube= read_mesh("data/cube.obj");
        
        // un autre objet
        m_objet= Mesh(GL_TRIANGLES);
        {
            // ajouter des triplets de sommet == des triangles dans objet...
        }
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest

        return 0;   // pas d'erreur, sinon renvoyer -1
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        m_objet.release();
        m_repere.release();
        return 0;   // pas d'erreur
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    // dessine le repere, place au centre du monde, pour le point de vue de la camera 
        draw(m_repere, /* model */ Identity(), camera());
        
    // dessine un cube, lui aussi place au centre du monde
        draw(m_cube, /* model */ Identity(), camera());
        
    // dessine le meme cube, a un autre endroit. 
        // il faut modifier la matrice model, qui sert a ca : placer un objet dans le monde, ailleurs qu'a l'origine.
        // par exemple, pour dessiner un 2ieme cube a la verticale au dessus du premier cube :
        // la transformation est une translation le long du vecteur Y= (0, 1, 0), si on veut placer le cube plus haut, il suffit d'utiliser une valeur > 1
        Transform t= Translation(0, 2, 0);
        draw(m_cube, /* model */ t, camera());
        
    // comment dessiner m_objet ?? 
    
    // et sans le superposer au cube deja dessine ?
        
        // continuer, afficher une nouvelle image
        // tant que la fenetre est ouverte...
        return 1;
    }

protected:
    Mesh m_objet;
    Mesh m_cube;
    Mesh m_repere;
};


int main( int argc, char **argv )
{
    // il ne reste plus qu'a creer un objet application et la lancer 
    TP tp;
    tp.run();
    
    return 0;
}
