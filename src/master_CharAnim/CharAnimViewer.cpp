
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "CharAnimViewer.h"

using namespace std;
using namespace chara;


CharAnimViewer::CharAnimViewer() : Viewer(), m_frameNumber(0)
{
}


int CharAnimViewer::init()
{
    Viewer::init();
    cout<<"==>master_CharAnim/CharAnimViewer"<<endl;
    m_camera.lookat( Point(0,0,0), 500 );
    gl.light( Point(300, 300, 300 ) );

    //b_draw_grid = false;

    m_part.resize( 10 );


    init_cylinder();
    init_sphere();


    //m_bvh.init("data/bvh/Robot.bvh" );
	m_bvh.init("data/bvh/danse.bvh");

    m_frameNumber = 0;
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<m_bvh<<endl;
    cout<<endl<<"========================"<<endl;

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


    draw_quad( RotationX(-90)*Scale(500,500,1) );

	// Affiche une pose du bvh
	bvhDrawGL(m_bvh, m_frameNumber);


    draw_particles();

    return 1;
}


int CharAnimViewer::update( const float time, const float delta )
{
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.

	if (key_state('n')) { m_frameNumber++; cout << m_frameNumber << endl; }
	if (key_state('b')) { m_frameNumber--; cout << m_frameNumber << endl; }

    m_part.update(0.1f);

    return 0;
}



void CharAnimViewer::draw_particles()
{
        int i;
        for(i=0;i<m_part.size();++i)
        {
            draw_sphere( m_part[i].position(), m_part[i].radius() );
        }
}
