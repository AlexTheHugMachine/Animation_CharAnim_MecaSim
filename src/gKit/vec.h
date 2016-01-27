
#ifndef _VEC_H
#define _VEC_H


struct vec2
{
    float x, y;
};

vec2 make_vec2( const float x, const float y );

vec2 operator+( const vec2& a, const vec2& b );
vec2 operator-( const vec2& a );
vec2 operator-( const vec2& a, const vec2& b );


struct vec3
{
    float x, y, z;
};

vec3 make_vec3( const float x, const float y, const float z );
vec3 make_vec3( const vec2& v, const float z );

vec3 operator+( const vec3& a, const vec3& b );
vec3 operator+( const vec3& a, const float b );
vec3 operator+( const float& a, const vec3& b );

vec3 operator-( const vec3& a );
vec3 operator-( const vec3& a, const vec3& b );
vec3 operator-( const vec3& a, const float& b );
vec3 operator-( const float& a, const vec3& b );

vec3 operator*( const vec3& a, const vec3& b );
vec3 operator*( const vec3& a, const float b );
vec3 operator*( const float a, const vec3& b );

vec3 operator/( const vec3& a, const vec3& b );
vec3 operator/( const vec3& a, const float b );
vec3 operator/( const float a, const vec3& b );

vec3 normalize( const vec3& a );
vec3 cross( const vec3& a, const vec3& b );
float dot( const vec3& a, const vec3& b );
float distance( const vec3& a, const vec3& b );
float length( const vec3& a );

// classes derivees... pas typedef !
//~ typedef vec3 point;
//~ typedef vec3 vector;
//~ typedef vec3 normal;

//~ point make_point( const float x, const float y, const float z );
//~ vector make_vector( const float x, const float y, const float z );
//~ vector make_vector( const point& a, const point& b );   // vecteur ab
//~ normal make_normal( const float x, const float y, const float z );

//~ point operator+( const point& p, const vector& v );
//~ point operator+( const vector& v, const point& p );

//~ vector operator-( const point& p, const vector& v );
//~ vector operator-( const vector& v, const point& p );
//~ vector operator-( const point& a, const point& b );     // vecteur ab


struct vec4
{
    float x, y, z, w;
};

vec4 make_vec4( const float x, const float y, const float z, const float w );
vec4 make_vec4( const vec2& v, const float z, const float w );
vec4 make_vec4( const vec3& v, const float w );

vec4 make_point4( const vec3& p );
vec4 make_vector4( const vec3& v );


#endif
