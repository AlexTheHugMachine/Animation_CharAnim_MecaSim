
#ifndef CHARANIMVIEWER_H
#define CHARANIMVIEWER_H

#include "../l2_lifgfx/Viewer.h"
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

    Mesh m_cylinder;
    Mesh m_cylinder_cover;
    Mesh m_sphere;
    void init_cylinder();
    void init_sphere();
    void draw_cylinder(const Transform& T);
    void draw_sphere(const Transform& T);

    Transform m_Tplane;



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
