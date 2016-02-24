
#ifndef _ORBITER_H
#define _ORBITER_H

#include "vec.h"
#include "mat.h"

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
//! cree une camera. observe le point \param center a une distance \param size.
Orbiter make_orbiter_lookat( const Point& center, const float size );

//! modifie une camera. observe le point \param center a une distance \param size.
void orbiter_lookat( Orbiter&o, const Point& center, const float size );

//! change le point de vue / la direction d'observation.
void orbiter_rotation( Orbiter&o, const float x, const float y );
//! deplace le centre / le point observe.
void orbiter_translation( Orbiter&o, const float x, const float y );
//! rapproche / eloigne la camera du centre.
void orbiter_move( Orbiter&o, const float z );

//! renvoie la transformation vue.
Transform orbiter_view_transform( const Orbiter&o );
//! renvoie la projection reglee pour une image d'aspect \param width / \param height, et une ouverture de \param fov degres.
Transform orbiter_projection_transform( const Orbiter&o, const float width, const float height, const float fov );

#endif
