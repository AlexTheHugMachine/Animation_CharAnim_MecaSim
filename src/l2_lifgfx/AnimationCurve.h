
#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <iostream>

#include "vec.h"
#include "mat.h"
#include "mesh.h"
#include "orbiter.h"


//! un tableau dynamique contenant les points d'une courbe d'animation
class AnimationCurve
{
public:
    AnimationCurve() : m_P(nullptr), m_nbp(0) {}

    //! initialise la courbe d'animation avec une liste de points de C du fichier "nom_fichier"
    void init(const char* nom_fichier);

    void draw(const Orbiter& camera);

    const Point& operator[]( const int i) { assert(i>=0); assert(i<m_nbp);  return m_P[i]; }
    int nb_points() const { return m_nbp; }

protected:
    //! Les points de la trajectoire/courbe d'animation que va suivre la fusée
    Point* m_P;
    //! Nombre de point
    int m_nbp;

    Mesh m_curve;

    Vector Bezier(const Vector& p0, const Vector& p1, const Vector& p2, const Vector& p3, const float t);
};



#endif
