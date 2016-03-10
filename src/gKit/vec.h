
#ifndef _VEC_H
#define _VEC_H

//! \addtogroup math
///@{

//! \file 
//! operations sur points et vecteurs

//! representation d'un point 3d.
struct Point
{
    float x, y, z;
};

//! cree un point de composantes x, y, z.
Point make_point( const float x, const float y, const float z );

//! renvoie la distance etre 2 points.
float distance( const Point& a, const Point& b );

//! renvoie le milieu du segment ab.
Point center( const Point& a, const Point& b );

//! representation d'un vecteur 3d.
struct Vector
{
    float x, y, z;
};

//! cree un vecteur de composantes x, y, z.
Vector make_vector( const float x, const float y, const float z );

//! renvoie le vecteur ab.
Vector make_vector( const Point& a, const Point& b );
//! renvoie le vecteur 0a.
Vector make_vector( const Point& a );
//! renvoie le vecteur a-b, ba
Vector operator- ( const Point& a, const Point& b );

//! renvoie le vecteur -v.
Vector operator- ( const Vector& v );

//! renvoie le point a+v.
Point operator+ ( const Point& a, const Vector& v );
//! renvoie le point a+v.
Point operator+ ( const Vector& v, const Point& a );
//! renvoie le point a-v.
Point operator- ( const Vector& v, const Point& a );
//! renvoie le point a-v.
Point operator- ( const Point& a, const Vector& v );
//! renvoie le vecteur u+v.
Vector operator+ ( const Vector& u, const Vector& v );
//! renvoie le vecteur u-v.
Vector operator- ( const Vector& u, const Vector& v );
//! renvoie le vecteur k*u;
Vector operator* ( const float k, const Vector& v );
//! renvoie le vecteur k*v;
Vector operator* ( const Vector& v, const float k );
//! renvoie le vecteur v/k;
Vector operator/ ( const Vector& v, const float k );

//! renvoie un vecteur unitaire / longueur == 1.
Vector normalize( const Vector& v );
//! renvoie le produit vectoriel de 2 vecteurs.
Vector cross( const Vector& u, const Vector& v );
//! renvoie le produit scalaire de 2 vecteurs.
float dot( const Vector& u, const Vector& v );
//! renvoie la longueur d'un vecteur.
float length( const Vector& v );


//! vecteur generique, utilitaire.
struct vec2
{
    float x, y;
};

vec2 make_vec2( const float x, const float y );

//! vecteur generique, utilitaire.
struct vec3
{
    float x, y, z;
};

vec3 make_vec3( const float x, const float y, const float z );
vec3 make_vec3( const vec2& v, const float z );

Point make_point( const vec3& p );
vec3 make_vec3( const Point& p );
Vector make_vector( const vec3& v );
vec3 make_vec3( const Vector& v );

//! vecteur generique 4d, ou 3d homogene, utilitaire.
struct vec4
{
    float x, y, z, w;
};

vec4 make_vec4( const float x, const float y, const float z, const float w );
vec4 make_vec4( const vec2& v, const float z, const float w );
vec4 make_vec4( const vec3& v, const float w );

//! construit un point 3d homogene (x, y, z, 1).
vec4 make_vec4( const Point& a );
//! construit un vecteur 3d homogene (x, y, z, 0).
vec4 make_vec4( const Vector& v );

///@}
#endif
