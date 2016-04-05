
#ifndef _MAT_H
#define _MAT_H

#include "vec.h"


//! \addtogroup math manipulations de points, vecteur, matrices, transformations
///@{

//! \file 
//! transformation de points et vecteurs

//! conversion en radians.
float radians( const float deg );
//! conversion en degres.
float degrees( const float rad );

//! representation d'une transformation, une matrice 4x4.
struct Transform
{
    float m[4][4];
};

//! construit la transformation identite.
Transform make_identity( );

//! construit la transformation. utilitaire.
Transform make_transform( 
    const float t00, const float t01, const float t02, const float t03,
    const float t10, const float t11, const float t12, const float t13,
    const float t20, const float t21, const float t22, const float t23,
    const float t30, const float t31, const float t32, const float t33 );

//! renvoie la transposee de la matrice.
Transform make_transpose( const Transform & m );
//! renvoie l'inverse de la matrice.
Transform make_inverse( const Transform& m );
//! renvoie la transformation a appliquer aux normales d'un objet transforme par la matrice m.
Transform make_normal_transform( const Transform& m );

//! renvoie la matrice representant une mise a l'echelle / etirement.
Transform make_scale( const float x, const float y, const float z );

//! renvoie la matrice representant une translation par un vecteur.
Transform make_translation( const Vector& v );
//! renvoie la matrice representant une translation par un vecteur x y z.
Transform make_translation( const float x, const float y, const float z );

//! renvoie la matrice representation une rotation de angle degree autour de l'axe X.
Transform make_rotationX( const float angle );
//! renvoie la matrice representation une rotation de a degree autour de l'axe Y.
Transform make_rotationY( const float angle );
//! renvoie la matrice representation une rotation de angle degree autour de l'axe Z.
Transform make_rotationZ( const float angle );
//! renvoie la matrice representation une rotation de angle degree autour de l'axe axis.
Transform make_rotation( const Vector& axis, const float angle );

//! renvoie la matrice representant une transformation viewport.
Transform make_viewport( const float width, const float height );
//! renvoie la matrice representant une transformation projection perspective.
Transform make_perspective( const float fov, const float aspect, const float znear, const float zfar );
//! renvoie la matrice representant le placement et l'orientation d'une camera pour observer le point to.
Transform make_lookat( const Point& from, const Point& to, const Vector& up );

//! renvoie la composition des transformations a et b, t= a * b.
Transform compose_transform( const Transform& a, const Transform& b );
//! renvoie la composition des transformations a et b, t = a * b.
Transform operator* ( const Transform& a, const Transform& b );

//! renvoie le point transforme.
Point transform( const Transform& m, const Point& p );
//! renvoie le vecteur transforme.
Vector transform( const Transform& m, const Vector& v );

//! renvoie le point/vecteur homogene transforme.
vec4 transform( const Transform& m, const vec4& v );

///@}
#endif
