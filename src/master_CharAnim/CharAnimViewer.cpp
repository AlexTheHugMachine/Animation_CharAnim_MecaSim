
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


    //m_bvh.init("data/bvh/Robot.bvh" );
	m_bvh.init("data/bvh/danse.bvh");

    m_frameNumber = 0;
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<m_bvh<<endl;
    cout<<endl<<"========================"<<endl;


    m_angle_a=0;
    m_angle_b=40;
    m_angle_milieu_ab = (m_angle_a+m_angle_b)/2;


    m_quat_a.setAxisAngleDegree(Vector(0,0,1),90);
    Transform R;
    m_quat_a.getMatrix44(R);
    cout<<R<<endl;

    R =RotationZ(90);
    cout<<R<<endl;
    return 0;
}



void CharAnimViewer::bvhDrawGL(const BVH&, int frameNumber)
{
	// TODO
}


void CharAnimViewer::bvhDrawGLRec(const BVHJoint&, int frameNumber, Transform& T) // la fonction récursive sur le squelette
{
	// TODO
}


int CharAnimViewer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Viewer::manageCameraLight();

    gl.camera(m_camera);

	// Affiche une pose du bvh
	bvhDrawGL(m_bvh, m_frameNumber);


	// affiche 3 cylindres dont l'angle est interpole
    draw_cylinder( Translation(5,0,0)*RotationZ(m_angle_a)*Scale(0.1,2,0.1) );
    draw_cylinder( Translation(5,0,0)*RotationZ(m_angle_b)*Scale(0.1,2,0.1) );
    draw_cylinder( Translation(5,0,0)*RotationZ(m_angle_milieu_ab)*Scale(0.1,2,0.1) );

	// affiche 3 cylindres dont le quaternion est interpole
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
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.

	if (key_state('n')) { m_frameNumber++; cout << m_frameNumber << endl; }
	if (key_state('b')) { m_frameNumber--; cout << m_frameNumber << endl; }


    m_angle_a = int(0.1*time)%360;
    m_angle_b = int(0.1*time+40)%360;
    m_angle_milieu_ab = (m_angle_a+m_angle_b)/2;

    Vector Z(0,0,1);
    m_quat_a.setAxisAngleDegree(Z,m_angle_a); //m_quat_a.invert();
    m_quat_b.setAxisAngleDegree(Z,m_angle_b); //m_quat_b.invert();
    m_quat_milieu_ab = Quaternion::slerp( m_quat_a, m_quat_b, 0.5);

    return 0;
}

