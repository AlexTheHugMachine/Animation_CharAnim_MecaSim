
#include <cmath>

#include "vec.h"


Point make_point( const float x, const float y, const float z )
{
    //~ Point p;
    //~ p.x= x; p.y= y; p.z= z;
    //~ return p;
    return Point(x, y, z);
}

float distance( const Point& a, const Point& b )
{
    return length(a - b);
}

float distance2( const Point& a, const Point& b )
{
    return length2(a - b);
}

Point center( const Point& a, const Point& b )
{
    //~ return make_point((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f, (a.z + b.z) * 0.5f);
    return Point((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f, (a.z + b.z) * 0.5f);
}


Vector make_vector( const float x, const float y, const float z )
{
    //~ Vector v;
    //~ v.x= x; v.y= y; v.z= z;
    //~ return v;
    return Vector(x, y, z);
}

Vector make_vector( const Point& a, const Point& b )
{
    //~ return b - a;
    return Vector(a, b);
}

Vector make_vector( const Point& a )
{
    //~ return make_vector(a.x, a.y, a.z);
    return Vector(a);
}

Vector operator- ( const Point& a, const Point& b )
{
    return make_vector(a.x - b.x, a.y - b.y, a.z - b.z);        //! \bug argh !! calcule le vecteur oppose... reprendre tous les tutos... !!
    //~ return Vector(a, b);
}

Vector operator- ( const Vector& v )
{
    //~ return make_vector(-v.x, -v.y, -v.z);
    return Vector(-v.x, -v.y, -v.z);
}

Point operator+ ( const Point& a, const Vector& v )
{
    //~ return make_point(a.x + v.x, a.y + v.y, a.z + v.z);
    return Point(a.x + v.x, a.y + v.y, a.z + v.z);
}

Point operator+ ( const Vector& v, const Point& a )
{
    return a + v;
}

Point operator- ( const Vector& v, const Point& a )
{
    return a + (-v);
}

Point operator- ( const Point& a, const Vector& v )
{
    return a + (-v);
}

Vector operator+ ( const Vector& u, const Vector& v )
{
    //~ return make_vector(u.x + v.x, u.y + v.y, u.z + v.z);
    return Vector(u.x + v.x, u.y + v.y, u.z + v.z);
}

Vector operator- ( const Vector& u, const Vector& v )
{
    //~ return make_vector(u.x - v.x, u.y - v.y, u.z - v.z);
    return Vector(u.x - v.x, u.y - v.y, u.z - v.z);
}

Vector operator* ( const float k, const Vector& v )
{
    //~ return make_vector(k * v.x, k * v.y, k * v.z);
    return Vector(k * v.x, k * v.y, k * v.z);
}

Vector operator* ( const Vector& v, const float k )
{
    return k * v;
}

Vector operator/ ( const Vector& v, const float k )
{
    float kk= 1 / k;
    return kk * v;
}

Vector normalize( const Vector& v )
{
    float kk= 1 / length(v);
    return kk * v;
}

Vector cross( const Vector& u, const Vector& v )
{
    return make_vector(
        (u.y * v.z) - (u.z * v.y),
        (u.z * v.x) - (u.x * v.z),
        (u.x * v.y) - (u.y * v.x));
}

float dot( const Vector& u, const Vector& v )
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

float length( const Vector& v )
{
    return std::sqrt(length2(v));
}

float length2( const Vector& v )
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}


vec2 make_vec2( const float x, const float y )
{
    //~ vec2 v;
    //~ v.x= x; v.y= y;
    //~ return v;
    return vec2(x, y);
}

vec3 make_vec3( const float x, const float y, const float z )
{
    //~ vec3 v;
    //~ v.x= x; v.y= y; v.z= z;
    //~ return v;
    return vec3(x, y, z);
}

vec3 make_vec3( const vec2& u, const float z )
{
    //~ vec3 v;
    //~ v.x= u.x; v.y= u.y; v.z= z;
    //~ return v;
    return vec3(u.x, u.y, z);
}

vec4 make_vec4( const float x, const float y, const float z, const float w )
{
    //~ vec4 v;
    //~ v.x= x; v.y= y; v.z= z; v.w= w;
    //~ return v;
    return vec4(x, y, z, w);
}

vec4 make_vec4( const vec2& u, const float z, const float w )
{
    //~ vec4 v;
    //~ v.x= u.x; v.y= u.y; v.z= z; v.w= w;
    //~ return v;
    return vec4(u.x, u.y, z, w);
}

vec4 make_vec4( const vec3& u, const float w )
{
    //~ vec4 v;
    //~ v.x= u.x; v.y= u.y; v.z= u.z; v.w= w;
    //~ return v;
    return vec4(u.x, u.y, u.z, w);
}


Point make_point( const vec3& p )
{
    //~ return make_point(p.x, p.y, p.z);
    return Point(p);
}

vec3 make_vec3( const Point& p )
{
    //~ return make_vec3(p.x, p.y, p.z);
    return vec3(p);
}

Vector make_vector( const vec3& v )
{
    //~ return make_vector(v.x, v.y, v.z);
    return Vector(v);
}

vec3 make_vec3( const Vector& v )
{
    //~ return make_vec3(v.x, v.y, v.z);
    return vec3(v);
}


vec4 make_vec4( const Point& a )
{
    //~ return make_vec4(a.x, a.y, a.z, 1);
    return vec4(a);
}

vec4 make_vec4( const Vector& v )
{
    //~ return make_vec4(v.x, v.y, v.z, 0);
    return vec4(v);
}

