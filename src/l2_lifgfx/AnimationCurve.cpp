#include <time.h>
#include <cassert>
#include <cstdio>

#include "draw.h"        // pour dessiner du point de vue d'une camera

#include "AnimationCurve.h"


const int NBPB=20;              // NomBre de Points de Bezier (entre 2 points de Contr il y aura NPB points)


//float temps()
//{
//    return float(clock())/CLOCKS_PER_SEC;
//}


// Fonction de calcul
// pc[0..3] sont les 4 points de controles
// t  le  parametre
// p le  point resultat de la courbe
Vector AnimationCurve::Bezier(const Vector& p0, const Vector& p1, const Vector& p2, const Vector& p3, const float t)
{
    float t1,t13,t3;
    Vector p, r0, r1, r2, r3;
    t1 = 1 - t;
    t13 = t1 * t1 * t1;
    t3 = t * t * t;
    p = t13*p0 + 3*t*t1*t1*p1 + 3*t*t*t1*p2 + t3*p3;
    return p;

//    vecMul( r0, t13, p0);
//    vecMul( r1, 3*t*t1*t1, p1);
//    vecMul( r2, 3*t*t*t1, p2);
//    vecMul( r3, t3, p3);
//    vecAdd( p, r0, r1);
//    vecAdd( p, p, r2);
//    vecAdd( p, p, r3);
//    return p;
}


void AnimationCurve::init(const char* nom_fichier)
{
    const int taille=512;
    char txt[taille];
    FILE* f;

    f = fopen( nom_fichier, "r");
    if (f==nullptr)
    {
        printf("ERREUR: ouverture impossible du fichier %s\n", nom_fichier);
        assert(f);
    }

    // donnée temporaire (=point de controle PC) servant pour calculer les points finaux (P)
    int NBPC;
    Point* PC;

    do
    {
        fgets( txt,taille,f);
    }
    while( txt[0]=='#' );
    int err = sscanf( txt, "%d\n", &NBPC);
    assert( err==1 );
    m_nbp = (NBPC/4)*NBPB;	// Nombre de points en tout

    PC = new Point[NBPC];
    m_P = new Point[m_nbp];
    int i;
    for(i=0; i<NBPC; ++i)
    {
        do
        {
            fgets( txt,taille,f);
        }
        while( txt[0]=='#' );
        err = sscanf( txt, "%f %f %f\n", &PC[i].x, &PC[i].y, &PC[i].z);
        assert( err==3 );
    }

    printf("Animation: %d PdC et %d P en tout\n", NBPC, m_nbp);
    //for(i=0;i<NBPC;++i) vecPrint( a.P[i] );


    // Courbe de Bezier
    int c=0, j;
    for (i=0; i<NBPC; i+=4)                 // Pour calculer une courbe de Bezier, on regroupe les points de controle par 4
    {
        for (j=0; j<NBPB; j++)
        {
            m_P[c] = Point(Bezier( Vector(PC[i]),
                                   Vector(PC[(i+1)%NBPC]), Vector(PC[(i+2)%NBPC]), Vector(PC[(i+3)%NBPC]),  float(j)/(NBPB) ) );
            c++;
        }
    }
    delete[] PC;



    m_curve = Mesh(GL_LINES);

    m_curve.color( Color(1, 0, 0));
    for(i=0; i<m_nbp-1; ++i)
    {
        m_curve.vertex( m_P[i] );
        m_curve.vertex( m_P[i+1] );
    }
    m_curve.vertex( m_P[i] );
    m_curve.vertex( m_P[0] );
}


    void AnimationCurve::draw(const Orbiter& camera)
    {
        ::draw( m_curve,  camera);
    }
