
#ifndef _VEC_H
#define _VEC_H


//! \addtogroup math
///@{

//! \file
//! operations sur points et vecteurs

//! representation d'un point 3d.
struct Point
{
    Point( const float _x= 0, const float _y= 0, const float _z= 0 ) : x(_x), y(_y), z(_z) {}
    
    float x, y, z;
};

//! cree un point de composantes x, y, z.
Point make_point( const float x, const float y, const float z );

//! renvoie la distance etre 2 points.
float distance( const Point& a, const Point& b );
//! renvoie le carre de la distance etre 2 points.
float distance2( const Point& a, const Point& b );

//! renvoie le milieu du segment ab.
Point center( const Point& a, const Point& b );

//! representation d'un vecteur 3d.
struct Vector
{
    Vector( const float _x= 0, const float _y= 0, const float _z= 0) : x(_x), y(_y), z(_z) {}
    
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
//! renvoie la carre de la longueur d'un vecteur.
float length2( const Vector& v );


//! vecteur generique, utilitaire.
struct vec2
{
    vec2( const float _x= 0, const float _y= 0 ) : x(_x), y(_y) {}
    
    float x, y;
};

//! cree un vecteur generique de composantes x, y.
vec2 make_vec2( const float x, const float y );

//! vecteur generique, utilitaire.
struct vec3
{
    vec3( const float _x= 0, const float _y= 0, const float _z= 0 ) : x(_x), y(_y), z(_z) {}
    
    float x, y, z;
};

//! cree un vecteur generique de composantes x, y, z.
vec3 make_vec3( const float x, const float y, const float z );
//! cree un vecteur generique de composantes x, y, z
vec3 make_vec3( const vec2& v, const float z );

//! cree un point de composantes p.x, p.y, p.z.
Point make_point( const vec3& p );
//! cree un vecteur generique de composantes p.x, p.y, p.z.
vec3 make_vec3( const Point& p );
//! cree un vecteur de composantes v.x, v.y, v.z.
Vector make_vector( const vec3& v );
//! cree un vecteur generique de composantes v.x, v.y, v.z.
vec3 make_vec3( const Vector& v );

//! vecteur generique 4d, ou 3d homogene, utilitaire.
struct vec4
{
    vec4( const float _x= 0, const float _y= 0, const float _z= 0, const float _w= 0 ) : x(_x), y(_y), z(_z), w(_w) {}
    float x, y, z, w;
};

//! cree un vecteur generique de composantes x, y, z, w.
vec4 make_vec4( const float x, const float y, const float z, const float w );
//! cree un vecteur generique de composantes v.x, v.y, z, w.
vec4 make_vec4( const vec2& v, const float z, const float w );
//! cree un vecteur generique de composantes v.x, v.y, v.z, w.
vec4 make_vec4( const vec3& v, const float w );

//! construit un point 3d homogene (x, y, z, 1).
vec4 make_vec4( const Point& a );
//! construit un vecteur 3d homogene (x, y, z, 0).
vec4 make_vec4( const Vector& v );

///@}
#endif
