
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

    //draw_quad( RotationX(-90)*Scale(500,500,1) );

    Viewer::manageCameraLight();
    gl.camera(m_camera);

    //m_world.draw();

	// Affiche le skeleton
    draw_skeleton( m_ske );

	draw_cylinder( Scale(10,100,10) );
	draw_sphere( Translation(20, 0, 0) * Scale(10,10,10) );



#if 0			// exercice du cours
	const int MAX = 5;
	float A[] = { -10, -20, -30, -40, -50 };
	float B[] = { -15, -10, 0, 10, 45 };
	float C[] = { -55, -30, 20, -30, -45 };

	static int t = 0;
	t = (t + 1) % MAX;
	float c = 0.5;
	float a = (1-c) * A[t] + c*A[(t+1)%MAX];
	float b = B[t];
	float ac = C[t];

	Transform S = Scale(10, 100, 10);
	Transform A2W = RotationZ(a);
	draw_cylinder( A2W*S );

	Transform B2A = Translation(0, 100, 0) * RotationZ( b);
	Transform B2W = A2W * B2A;
	draw_cylinder(  B2W*S);

	Transform C12B = Translation(0, 100, 0) * RotationZ(ac);
	Transform C12W = B2W * C12B;
	Transform S1 = Scale(6, 30, 6);
	draw_cylinder(C12W*S1);

	Transform C22B = Translation(0, 100, 0) * RotationZ(-ac);
	Transform C22W = B2W * C22B;
	draw_cylinder(C22W*S1);
#endif


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



