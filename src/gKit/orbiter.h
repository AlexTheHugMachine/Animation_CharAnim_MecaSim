
#ifndef _ORBITER_H
#define _ORBITER_H

#include "vec.h"
#include "mat.h"


struct Orbiter
{
    Point center;
    vec2 position;
    vec2 rotation;
    float size;
};

Orbiter make_orbiter( );
Orbiter make_orbiter_lookat( const Point& center, const float size );

void orbiter_lookat( Orbiter&o, const Point& center, const float size );

void orbiter_rotation( Orbiter&o, const float x, const float y );
void orbiter_translation( Orbiter&o, const float x, const float y );
void orbiter_move( Orbiter&o, const float z );

Transform orbiter_view_transform( const Orbiter&o );
//! renvoie la projection reglee pour une image d'aspect \param width / \param height, et une ouverture de \param fov degres.
Transform orbiter_projection_transform( const Orbiter&o, const float width, const float height, const float fov );

//! projection par defaut: aspect 1, fov 45
Transform orbiter_projection_transform( const Orbiter&o );

#endif
