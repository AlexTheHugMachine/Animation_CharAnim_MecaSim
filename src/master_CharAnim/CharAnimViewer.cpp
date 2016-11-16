
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "CharAnimViewer.h"

using namespace std;



void CharAnimViewer::init_sphere()
{
    const int divBeta = 26;
    const int divAlpha = divBeta/2;
    int i,j;
    float beta, alpha, alpha2;

    m_sphere = Mesh(GL_TRIANGLE_STRIP);

    m_sphere.color( Color(1, 1, 1) );

    for(i=0;i<divAlpha;++i)
    {
        alpha = -0.5f*M_PI + float(i)*M_PI/divAlpha;
        alpha2 = -0.5f*M_PI + float(i+1)*M_PI/divAlpha;

        for(j=0;j<divBeta;++j)
        {
            beta = float(j)*2.f*M_PI/(divBeta-1);

            m_sphere.texcoord(beta/(2.0f*M_PI), 0.5f+alpha/M_PI);
            m_sphere.normal( Vector(cos(alpha)*cos(beta),  sin(alpha), cos(alpha)*sin(beta)) );
            m_sphere.vertex( Point(cos(alpha)*cos(beta),  sin(alpha), cos(alpha)*sin(beta)) );


            m_sphere.texcoord(beta/(2.0f*M_PI), 0.5f+alpha2/M_PI);
            m_sphere.normal( Vector(cos(alpha2)*cos(beta),  sin(alpha2), cos(alpha2)*sin(beta)) );
            m_sphere.vertex( Point(cos(alpha2)*cos(beta),  sin(alpha2), cos(alpha2)*sin(beta))   );
        }

        m_sphere.restart_strip();
    }
}



void CharAnimViewer::init_cylinder()
{
    int i;
    const int div = 25;
    float alpha;
    float step = 2.0*M_PI / (div);

    m_cylinder = Mesh(GL_TRIANGLE_STRIP);

    for (i=0;i<=div;++i)
    {
        alpha = i*step;
        m_cylinder.normal( Vector(cos(alpha),  0, sin(alpha)) );
        m_cylinder.texcoord(float(i)/div, 0.f );
        m_cylinder.vertex( Point(cos(alpha),  0, sin(alpha)));

        m_cylinder.normal( Vector(cos(alpha),  0, sin(alpha)) );
        m_cylinder.texcoord(float(i)/div, 1.f );
        m_cylinder.vertex( Point(cos(alpha),   1, sin(alpha)));
    }

    m_cylinder_cover = Mesh( GL_TRIANGLE_FAN );

    m_cylinder_cover.normal( Vector(0,1,0) );

    m_cylinder_cover.vertex( Point(0,0,0));
    for (i=0;i<=div;++i)
    {
        alpha = -i*step;
        m_cylinder_cover.vertex( Point(cos(alpha),  0, sin(alpha)));
    }
}

void CharAnimViewer::draw_cylinder(const Transform& T)
{
    gl.model( T );
    gl.draw( m_cylinder );

    Transform Tch = T * Translation( 0, 1, 0);
    gl.model( Tch );
    gl.draw( m_cylinder_cover );

    //Transform Tcb = T  * Translation( 0, -1, 0);
    Transform Tcb = T * Translation( 0, 0, 0) * Rotation( Vector(1,0,0), 180);
    gl.model( Tcb );
    gl.draw( m_cylinder_cover );

}


void CharAnimViewer::draw_sphere(const Transform& T)
{
    gl.model( T );
    gl.draw( m_sphere );
}


int CharAnimViewer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Viewer::manageCameraLight();

    gl.camera(m_camera);


    Transform T = Translation( 1, 1, 1 );
    // ===CYLINDRE
    draw_cylinder( T*Scale(0.1,2,0.1) );
    draw_cylinder( T*Translation(0,2,0)*RotationZ(45)*Scale(0.1,2,0.1) );

    // ===SPHERE
    draw_sphere( T*Translation(0,0,0)*Scale(0.2,0.2,0.2) );
    draw_sphere( T*Translation(0,2,0)*Scale(0.2,0.2,0.2) );


    return 1;
}


int CharAnimViewer::update( const float time, const float delta )
{
    // modifier l'orientation du cube a chaque image.
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.

    m_Tplane = RotationY( degrees(time) );



    return 0;
}



CharAnimViewer::CharAnimViewer() : Viewer()
{
}



int CharAnimViewer::init()
{

    Viewer::init();
    m_camera.lookat( Point(0,0,0), 50 );

    init_cylinder();
    init_sphere();


    m_bvh.init("data/animation/bvh/Robot.bvh" );
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<m_bvh<<endl;
    cout<<endl<<"========================"<<endl;

    return 0;
}
