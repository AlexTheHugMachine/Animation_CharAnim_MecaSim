
#ifndef _ORBITER_H
#define _ORBITER_H

#include "vec.h"
#include "mat.h"


//! \addtogroup objet3D
///@{

//! \file 
//! camera pour observer un objet.

//! representation de la camera, type orbiter, placee sur une sphere autour du centre de l'objet.
struct Orbiter
{
    Point center;
    vec2 position;
    vec2 rotation;
    float size;
};

//! cree une camera par defaut. observe le centre (0, 0, 0) a une distance 5.
Orbiter make_orbiter( );
//! cree une camera. observe le point center a une distance size.
Orbiter make_orbiter_lookat( const Point& center, const float size );

//! modifie une camera. observe le point center a une distance size.
void orbiter_lookat( Orbiter&o, const Point& center, const float size );

//! change le point de vue / la direction d'observation.
void orbiter_rotation( Orbiter&o, const float x, const float y );
//! deplace le centre / le point observe.
void orbiter_translation( Orbiter&o, const float x, const float y );
//! rapproche / eloigne la camera du centre.
void orbiter_move( Orbiter&o, const float z );

//! renvoie la transformation vue.
Transform orbiter_view_transform( const Orbiter&o );
//! renvoie la projection reglee pour une image d'aspect width / height, et une ouverture de fov degres.
Transform orbiter_projection_transform( const Orbiter&o, const float width, const float height, const float fov );

/*! renvoie les coorodnnees de l'origine d0 et les axes dx, dy du plan image dans le repere du monde. 

permet de construire un rayon pour le pixel x, y : 
    - l'extremite : un point dans le plan image avec z = 0 : 
    \code
        Point d0;
        Vector dx0, dy0;
        orbiter_image_frame(camera, width, height, 0, fov, d0, dx0, dy0);
        Point e= d0 + x*dx0 + y*dy0;
    \endcode
    - l'origine : 
    \code
        Point o= orbiter_position(camera);
    \endcode
    
ou autre solution, reproduire la projection openGL :
    - extremite : un point dans le plan image avec z = 1 :  
    \code
        Point d1;
        Vector dx1, dy1;
        orbiter_image_frame(camera, width, height, 1, fov, d1, dx1, dy1);
        Point e= d1 + x*dx1 + y*dy1;
    \endcode
    - origine : un point dans le plan image avec z = 0 :
    \code
        Point d0;
        Vector dx0, dy0;
        orbiter_image_frame(camera, width, height, 0, fov, d0, dx0, dy0);
        Point o= d0 + x*dx0 + y*dy0;
    \endcode
 */
void orbiter_image_frame( const Orbiter& o, const float width, const float height, const float z, const float fov, Point& dO, Vector& dx, Vector& dy );

//! renvoie la position de la camera dans le repere du monde.
Point orbiter_position( const Orbiter& o );

//! relit la position de l'orbiter depuis un fichier texte. 
Orbiter read_orbiter( const char *filename );

//! enregistre la position de l'orbiter dans un fichier texte.
int write_orbiter( const Orbiter& o, const char *filename );

///@}
#endif
