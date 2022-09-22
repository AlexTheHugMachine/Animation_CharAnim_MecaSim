
#ifndef VIEWER_ETUDIANT_H
#define VIEWER_ETUDIANT_H

#include "Viewer.h"



class ViewerEtudiant : public Viewer
{
public:
    ViewerEtudiant();

    int init();
    int render();
    int update( const float time, const float delta );

protected:
    
    /* -----------------------------------------
     Pour creer un nouvel objet vous devez :
     
     1. declarer ici dans le fichier Viewer_etudiant.h
     le Mesh,
     la texture si besoin,
     une fonction 'init_votreObjet'
     une fonction 'draw_votreObjet(const Transform& T)'
     
     --- Rq : regarder comment cela est effectue dans le fichier Viewer.h
     
     
     2. Appeler la fonction 'init_votreObjet' dans la fonction 'init' du Viewer_etudiant.cpp
     --- Rq : regarder comment cela est effectue dans le fichier Viewer.cpp
     
     
     3. Appeler la fonction 'draw_votreObjet' dans la fonction 'render' du Viewer_etudiant.cpp
     --- Rq : regarder comment cela est effectue dans le fichier Viewer.cpp
     
      ----------------------------------------- */
    
    
    /// Declaration des Mesh

    
    /// Declaration des Textures
    
    
    /// Declaration des fonction de creation de Mesh du type init_votreObjet()
    
    
    /// Declaration des fonctions draw_votreObjet(const Transform& T)

    
};



#endif
