
#ifndef _MAT_H
#define _MAT_H

#include "vec.h"


float radians( const float deg );
float degrees( const float rad );

struct mat4
{
    float m[4][4];
    //! \todo minv, separer le type transform et le type mat4
};

mat4 make_identity( );

mat4 make_matrix( const vec4& r0, const vec4& r1, const vec4& r2, const vec4& r3 );

mat4 make_matrix( 
    const float t00, const float t01, const float t02, const float t03,
    const float t10, const float t11, const float t12, const float t13,
    const float t20, const float t21, const float t22, const float t23,
    const float t30, const float t31, const float t32, const float t33 );
    
mat4 make_transpose( const mat4& m );
mat4 make_inverse( const mat4& m );

mat4 make_normal_matrix( const mat4& m );

mat4 make_scale( const float x, const float y, const float z );

mat4 make_translation( const vec3& v );

mat4 make_rotationX( const float a );
mat4 make_rotationY( const float a );
mat4 make_rotationZ( const float a );
mat4 make_rotation( const vec3& axis, const float angle );

mat4 make_viewport( const float width, const float height );
mat4 make_perspective( const float fov, const float aspect, const float znear, const float zfar );
mat4 make_lookat( const vec3& from, const vec3& to, const vec3& up );

mat4 make_transform( const mat4& a, const mat4& b );

mat4 operator* ( const mat4& a, const mat4& b );

vec3 transform_point( const mat4& m, const vec3& p );
vec3 transform_vector( const mat4& m, const vec3& v );
vec3 transform_normal( const mat4& m, const vec3& n );

//! transforme un point/vecteur homogene: vec4 q= M(p); avec vec4 p; et mat4 M; 
//! utiliser cet operateur permet d'eviter les problemes d'assiciativite de *, par exemple vec4 q= P * V * M * p ? 
//! avec cette solution, pas d'ambiguite, vec4 q= (P * V * M)(p) 
//~ vec4 operator() ( const mat4& m, const vec4& v );   //!< \todo doit etre un membre de mat4 

vec4 transform( const mat4& m, const vec4& v );


#endif
