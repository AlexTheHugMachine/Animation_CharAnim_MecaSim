
#include <math.h>

#include "vec.h"


vec2 make_vec2( const float x, const float y )
{
    vec2 v;
    v.x= x;
    v.y= y;
    return v;
}


vec2 operator+( const vec2& a, const vec2& b )
{
    return make_vec2(a.x+b.x, a.y+b.y);
}

vec2 operator-( const vec2& a )
{
    return make_vec2(-a.x, -a.y);
}

vec2 operator-( const vec2& a, const vec2& b )
{
    return make_vec2(a.x-b.x, a.y-b.y);
}



vec3 make_vec3( const float x, const float y, const float z )
{
    vec3 v;
    v.x= x;
    v.y= y;
    v.z= z;
    return v;
}

vec3 make_vec3( const vec2& u, const float z )
{
    vec3 v;
    v.x= u.x;
    v.y= u.y;
    v.z= z;
    return v;
}


vec3 normalize( const vec3& a )
{
    float w= 1 / length(a);
    return make_vec3(a.x*w, a.y*w, a.z*w); 
}
    
vec3 cross( const vec3& a, const vec3& b )
{
    return make_vec3(
        ( a.y*b.z ) - ( a.z*b.y ),
        ( a.z*b.x ) - ( a.x*b.z ),
        ( a.x*b.y ) - ( a.y*b.x ));
}


float dot( const vec3& a, const vec3& b )
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

float distance( const vec3& a, const vec3& b )
{
    return length( a - b );
}

float length( const vec3& a )
{
    return sqrtf( a.x*a.x + a.y*a.y + a.z*a.z );
}


vec3 operator+( const vec3& a, const vec3& b )
{
    return make_vec3( a.x+b.x, a.y+b.y, a.z+b.z );
}

vec3 operator+( const vec3& a, const float b )
{
    return make_vec3( a.x+b, a.y+b, a.z+b );
}

vec3 operator+( const float& a, const vec3& b )
{
    return make_vec3( a+b.x, a+b.y, a+b.z );
}

vec3 operator-( const vec3& a )
{
    return make_vec3( -a.x, -a.y, -a.z );
}

vec3 operator-( const vec3& a, const vec3& b )
{
    return make_vec3( a.x-b.x, a.y-b.y, a.z-b.z );
}

vec3 operator-( const vec3& a, const float& b )
{
    return make_vec3( a.x-b, a.y-b, a.z-b );
}

vec3 operator-( const float& a, const vec3& b )
{
    return make_vec3( a-b.x, a-b.y, a-b.z );
}


vec3 operator*( const vec3& a, const vec3& b )
{
    return make_vec3( a.x*b.x, a.y*b.y, a.z*b.z );
}

vec3 operator*( const vec3& a, const float b )
{
    return make_vec3( a.x*b, a.y*b, a.z*b );
}

vec3 operator*( const float a, const vec3& b )
{
    return make_vec3( a*b.x, a*b.y, a*b.z );
}


vec3 operator/( const vec3& a, const vec3& b )
{
    return make_vec3( a.x/b.x, a.y/b.y, a.z/b.z );
}

vec3 operator/( const vec3& a, const float b )
{
    return make_vec3( a.x/b, a.y/b, a.z/b );
}

vec3 operator/( const float a, const vec3& b )
{
    return make_vec3( a/b.x, a/b.y, a/b.z );
}


vec4 make_vec4( const float x, const float y, const float z, const float w )
{
    vec4 v;
    v.x= x;
    v.y= y;
    v.z= z;
    v.w= w;
    return v;
}

vec4 make_vec4( const vec2& u, const float z, const float w )
{
    vec4 v;
    v.x= u.x;
    v.y= u.y;
    v.z= z;
    v.w= w;
    return v;
}

vec4 make_vec4( const vec3& u, const float w )
{
    vec4 v;
    v.x= u.x;
    v.y= u.y;
    v.z= u.z;
    v.w= w;
    return v;
}

vec4 make_point4( const vec3& p )
{
    return make_vec4(p, 1);
}

vec4 make_vector4( const vec3& v )
{
    return make_vec4(v, 0);
}


//~ point make_point( const float x, const float y, const float z )
//~ {
    //~ return make_vec3(x, y, z);
//~ }

//~ vector make_vector( const float x, const float y, const float z )
//~ {
    //~ return make_vec3(x, y, z);
//~ }


//~ point operator+( const point& p, const vector& v )
//~ {
    //~ return make_point( p.x+v.x, p.y+v.y, p.z+v.z );
//~ }

//~ point operator+( const vector& v, const point& p )
//~ {
    //~ return make_point( p.x+v.x, p.y+v.y, p.z+v.z );
//~ }

//~ vector operator-( const point& p, const vector& v )
//~ {
    //~ return make_vector( p.x-v.x, p.y-v.y, p.z-v.z );
//~ }

//~ vector operator-( const vector& v, const point& p )
//~ {
    //~ return make_vector( p.x-v.x, p.y-v.y, p.z-v.z );
//~ }

//~ vector operator-( const point& a, const point& b )
//~ {
    //~ return make_vector( b.x-a.x, b.y-a.y, b.z-a.z );
//~ }

