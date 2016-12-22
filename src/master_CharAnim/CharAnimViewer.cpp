
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "CharAnimViewer.h"

using namespace std;


CharAnimViewer::CharAnimViewer() : Viewer(), m_frameNumber(0)
{
}


int CharAnimViewer::init()
{
    Viewer::init();
    cout<<"==>master_CharAnim/CharAnimViewer"<<endl;
    m_camera.lookat( Point(0,0,0), 50 );

    init_cylinder();
    init_sphere();


    m_bvh.init("data/bvh/Robot.bvh" );
    m_frameNumber = 0;
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<m_bvh<<endl;
    cout<<endl<<"========================"<<endl;

    m_angle_a=0;
    m_angle_b=40;
    m_angle_milieu_ab = (m_angle_a+m_angle_b)/2;

    return 0;
}


int CharAnimViewer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Viewer::manageCameraLight();

    gl.camera(m_camera);

    if (key_state('n')) { m_frameNumber++; cout<<m_frameNumber<<endl; }
    if (key_state('b')) { m_frameNumber--; cout<<m_frameNumber<<endl; }


    draw_cylinder( Translation(5,0,0)*RotationZ(m_angle_a)*Scale(0.1,2,0.1) );
    draw_cylinder( Translation(5,0,0)*RotationZ(m_angle_b)*Scale(0.1,2,0.1) );
    draw_cylinder( Translation(5,0,0)*RotationZ(m_angle_milieu_ab)*Scale(0.1,2,0.1) );


    Transform R;

    m_quat_a.getMatrix44(R);
    draw_cylinder( Translation(-5,0,0)*R*Scale(0.1,2,0.1) );

    m_quat_b.getMatrix44(R);
    draw_cylinder( Translation(-5,0,0)*R*Scale(0.1,2,0.1) );

    m_quat_milieu_ab.getMatrix44(R);
    draw_cylinder( Translation(-5,0,0)*R*Scale(0.1,2,0.1) );



    return 1;
}


int CharAnimViewer::update( const float time, const float delta )
{
    // modifier l'orientation du cube a chaque image.
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.

    //m_T = RotationZ( degrees(0.01*time) );

    //m_q = Quaternion( Vector(0,1,1), degrees(0.01*time) ); // rotation
    //m_q = m_q * Quaternion( Vector(0,1,1), degrees(0.01*delta) ); // rotation

    m_angle_a = int(0.1*time)%360;
    m_angle_b = int(0.1*time+40)%360;
    m_angle_milieu_ab = (m_angle_a+m_angle_b)/2;

    m_quat_a = Quaternion( Vector(0,0,1), m_angle_a);
    m_quat_b = Quaternion( Vector(0,0,1), m_angle_b);
    m_quat_milieu_ab = Quaternion::slerp( m_quat_a, m_quat_b, 0.5);

    return 0;
}

