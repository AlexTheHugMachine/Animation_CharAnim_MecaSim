
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "CharAnimViewer.h"

using namespace std;
using namespace chara;


CharAnimViewer* CharAnimViewer::psingleton = NULL;


CharAnimViewer::CharAnimViewer() : Viewer(), m_frameNumber(0)
{
	psingleton = this;
}


int CharAnimViewer::init()
{
    Viewer::init();
    cout<<"==>master_CharAnim/CharAnimViewer"<<endl;
    m_camera.lookat( Point(0,100,10), 1000 );
    gl.light( Point(300, 300, 300 ) );

    //b_draw_grid = false;

    m_world.setParticlesCount( 10 );


    init_cylinder();
    init_sphere();


    //m_bvh.init("data/bvh/Robot.bvh" );
	m_bvh.init( smart_path("data/bvh/danse.bvh") );

    m_frameNumber = 0;
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<m_bvh<<endl;
    cout<<endl<<"========================"<<endl;

    //m_ske.init( m_bvh );
    //m_ske.setPose( m_bvh, -1);// met le skeleton a la pose au repos

    return 0;
}



void CharAnimViewer::draw_skeleton(const Skeleton& )
{
    // TODO
}



int CharAnimViewer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_quad( RotationX(-90)*Scale(500,500,1) );

    Viewer::manageCameraLight();
    gl.camera(m_camera);

    m_world.draw();

	// Affiche le skeleton
    draw_skeleton( m_ske );

    return 1;
}


int CharAnimViewer::update( const float time, const float delta )
{
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.

	if (key_state('n')) { m_frameNumber++; cout << m_frameNumber << endl; }
	if (key_state('b')) { m_frameNumber--; cout << m_frameNumber << endl; }

	m_ske.setPose( m_bvh, m_frameNumber );

    m_world.update(0.1f);

    return 0;
}



