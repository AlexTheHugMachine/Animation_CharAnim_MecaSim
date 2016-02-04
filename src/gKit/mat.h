
#ifndef _MAT_H
#define _MAT_H

#include "vec.h"


float radians( const float deg );
float degrees( const float rad );

struct Transform
{
    float m[4][4];
};

Transform make_identity( );

Transform make_transform( 
    const float t00, const float t01, const float t02, const float t03,
    const float t10, const float t11, const float t12, const float t13,
    const float t20, const float t21, const float t22, const float t23,
    const float t30, const float t31, const float t32, const float t33 );
    
Transform make_transpose( const Transform & m );
Transform make_inverse( const Transform& m );

Transform make_normal_transform( const Transform& m );

Transform make_scale( const float x, const float y, const float z );

Transform make_translation( const Vector& v );

Transform make_rotationX( const float a );
Transform make_rotationY( const float a );
Transform make_rotationZ( const float a );
Transform make_rotation( const Vector& axis, const float angle );

Transform make_viewport( const float width, const float height );
Transform make_perspective( const float fov, const float aspect, const float znear, const float zfar );
Transform make_lookat( const Point& from, const Point& to, const Vector& up );

Transform compose_transform( const Transform& a, const Transform& b );

Transform operator* ( const Transform& a, const Transform& b );

Point transform_point( const Transform& m, const Point& p );
Vector transform_vector( const Transform& m, const Vector& v );

#endif
