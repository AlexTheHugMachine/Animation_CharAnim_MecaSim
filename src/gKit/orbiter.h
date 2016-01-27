
#ifndef _ORBITER_H
#define _ORBITER_H

#include "vec.h"
#include "mat.h"


struct orbiter
{
    vec3 position;
    vec2 rotation;
    float size;
};

orbiter make_orbiter( );
orbiter make_orbiter_lookat( const vec3 position, const float size );

void orbiter_lookat( orbiter&o, const vec3 position, const float size );

void orbiter_rotation( orbiter&o, const float x, const float y );
void orbiter_translation( orbiter&o, const float x, const float y );
void orbiter_move( orbiter&o, const float z );

mat4 orbiter_view_matrix( const orbiter&o );
mat4 orbiter_projection_matrix( const orbiter&o, const float width, const float height, const float fov );

#endif
