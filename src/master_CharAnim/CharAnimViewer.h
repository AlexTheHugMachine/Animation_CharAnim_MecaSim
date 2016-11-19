
#ifndef CHARANIMVIEWER_H
#define CHARANIMVIEWER_H

#include "Viewer.h"
#include "BVH.h"

class CharAnimViewer : public Viewer
{
public:
    CharAnimViewer();

    int init();
    int render();
    int update( const float time, const float delta );


protected:

    bvh::BVH m_bvh;


    Transform m_T;



    /* Pour creer un nouvel objet vous devez :
       - declarer ici le Mesh
       - la texture si besoin
       - une fonction init_votreObjet et l'appeller dans la fonction init du .cpp
       - ajouter un appel a l'affichage dans la fonction draw
    */
//    Mesh quad;
//    GLuint quad_texture;
//    void init_quad();
};



#endif
