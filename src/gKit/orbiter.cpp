
#include "orbiter.h"


orbiter make_orbiter( )
{
    orbiter o;
    o.center= make_vec3(0, 0, 0);
    o.position= make_vec2(0, 0);
    o.rotation= make_vec2(0, 0);
    o.size= 5;
    return o;
}

orbiter make_orbiter_lookat( const vec3& center, const float size )
{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    orbiter o;
    o.center= center;
    o.position= make_vec2(0, 0);
    o.rotation= make_vec2(0, 0);
    o.size= size;
    return o;
}

void orbiter_rotation( orbiter&o, const float x, const float y )
{
    o.rotation= o.rotation + make_vec2(y, x);
}

void orbiter_translation( orbiter&o, const float x, const float y )
{
    o.position= o.position + make_vec2(-o.size * x, o.size * y);
}

void orbiter_move( orbiter&o, const float z )
{
    o.size= o.size - o.size * 0.01f * z;
    if(o.size < 0.001f)
        o.size= 0.001f;
}

void orbiter_lookat( orbiter&o, const vec3& center, const float size )
{
    o= make_orbiter_lookat(center, size);
}

mat4 orbiter_view_matrix( const orbiter&o )
{
    return make_translation(-make_vec3(o.position, o.size)) * make_rotationX(o.rotation.x) * make_rotationY(o.rotation.y) * make_translation( -o.center ); 
}

mat4 orbiter_projection_matrix( const orbiter&o, const float width, const float height, const float fov )
{
    return make_perspective(fov, width / height, o.size*0.01f, o.size*2.f + o.center.z);
}
