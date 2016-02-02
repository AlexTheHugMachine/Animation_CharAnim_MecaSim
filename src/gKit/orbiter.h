
#ifndef _ORBITER_H
#define _ORBITER_H

#include "vec.h"
#include "mat.h"


struct Orbiter
{
    vec3 center;
    vec2 position;
    vec2 rotation;
    float size;
};

Orbiter make_orbiter( );
Orbiter make_orbiter_lookat( const vec3& center, const float size );

void orbiter_lookat( Orbiter&o, const vec3& center, const float size );

void orbiter_rotation( Orbiter&o, const float x, const float y );
void orbiter_translation( Orbiter&o, const float x, const float y );
void orbiter_move( Orbiter&o, const float z );

mat4 orbiter_view_matrix( const Orbiter&o );
mat4 orbiter_projection_matrix( const Orbiter&o, const float width, const float height, const float fov );

#endif
