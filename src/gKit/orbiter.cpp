
#include "orbiter.h"


orbiter make_orbiter( )
{
    orbiter o;
    o.position= make_vec3(0, 0, 10);
    o.rotation= make_vec2(0, 0);
    o.size= 10;
    return o;
}

orbiter make_orbiter_lookat( const vec3 position, const float size )
{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    orbiter o;
    o.position= position;
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
    o.position= o.position + make_vec3(-o.position.z * x, o.position.z * y, 0);
}

void orbiter_move( orbiter&o, const float z )
{
    o.position.z= o.position.z - o.position.z * 0.01f * z;
    if(o.position.z < 0.001f)
        o.position.z= 0.001f;
}

void orbiter_lookat( orbiter&o, const vec3 position, const float size )
{
    o= make_orbiter_lookat(position, size);
}

mat4 orbiter_view_matrix( const orbiter&o )
{
    return make_translation(-o.position) * make_rotationX(o.rotation.x) * make_rotationY(o.rotation.y) * make_translation( make_vec3(0, 0, -o.size) ) ;
}

mat4 orbiter_projection_matrix( const orbiter&o, const float width, const float height, const float fov )
{
    return make_perspective(fov, width / height, 0.1, 1000);    //!< \todo calculer znear et zfar en fonction de position et size
}
