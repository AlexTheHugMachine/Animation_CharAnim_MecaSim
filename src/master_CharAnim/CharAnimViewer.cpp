
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
    m_camera.lookat( Point(0,0,0), 1000 );
	m_camera.rotation(180, 0);
    gl.light( Point(300, 300, 300 ) );

    //b_draw_grid = false;

    m_world.setParticlesCount( 10 );


    init_cylinder();
    init_sphere();


    m_bvh.init( smart_path("data/bvh/Robot.bvh") );
	//m_bvh.init( smart_path("data/bvh/danse.bvh") );

    m_frameNumber = 0;
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<m_bvh<<endl;
    cout<<endl<<"========================"<<endl;

    m_ske.init( m_bvh );
    m_ske.setPose( m_bvh, -1);// met le skeleton a la pose au repos

    return 0;
}



void CharAnimViewer::draw_skeleton(const Skeleton& )
{
    // TODO

	const int N = 4;
	static float angleA[N] = { 0, 10, 20, 10 };
	static float angleB[N] = { 30, -10, -20, -25 };
	static float t = 0;



	int et = int(t);
	float q = t - et;
	float a = q * angleA[(et+1)%N] + (1 - q) * angleA[(et) % N];
	Transform  a2w = Translation(10, 10, 0) * RotationZ(  a );
	draw_sphere(   a2w*Scale( 2,2,2)   );
	draw_cylinder(a2w * RotationZ(-90) * Scale(1,10,1) );

	float b = q * angleB[(et + 1) % N] + (1 - q) * angleB[(et) % N];
	Transform b2a = Translation(10, 0, 0) * RotationZ(b);
	draw_sphere(  a2w*b2a * Scale(2, 2, 2));
	draw_cylinder( a2w * b2a * RotationZ(-90) * Scale(1, 10, 1));

	Transform c2b = Translation(10, 0, 0);
	draw_sphere(a2w * b2a * c2b * Scale(2, 2, 2));

	t += 0.01;
	if (t >= N) t = 0.f;
	//sleep(100);

	//draw_cylinder( );



	//Transform a2w = Translation(10, 10, 0) ;
	//draw_sphere( a2w*Scale(2,2,2));
	//draw_cylinder(a2w * RotationZ(-90)*Translation(0, 1, 0)*Scale(1,10,1) );

	//Transform b2a = Translation(10, 0, 0);
	//Transform b2w = a2w * b2a;
	//draw_sphere( b2w * Scale(2, 2, 2));
	//draw_cylinder(b2w * RotationZ(-90) * Translation(0, 1, 0) * Scale(1, 10, 1));


}



int CharAnimViewer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw_quad( RotationX(-90)*Scale(500,500,1) );

    Viewer::manageCameraLight();
    gl.camera(m_camera);

	// Affiche les particules physiques (Question 3 : interaction personnage sphere/ballon)
    //m_world.draw();


	// Affiche le skeleton � partir de la structure lin�aire (tableau) Skeleton
    draw_skeleton( m_ske );







#if 0			// exercice du cours
	float A[] = { 0, -10, -20, -30, -40 };
	float B[] = { 0, -10, -20, -30, -40 };
	float C[] = { 0, -10, -20, -30, -40 };
	float D[] = { 0, 10, 20, 30, 40 };
	float a, b, c, d;

	static int t = 3;
	if (t == 4) t = 0; else t++;

	a = A[t];
	b = B[t];
	c = C[t];
	d = D[t];

	Transform scaleS = Scale(12, 12, 12);
	Transform scaleA = Scale(20, 20, 20);
	Transform scale = Scale(10, 100, 10);
	Transform A2W = RotationZ(a);
	draw_cylinder(A2W*scale);
	draw_sphere(A2W*scaleS);
	draw_axe(A2W*scaleA);

	Transform B2A = Translation(0, 100, 0) * RotationZ(b);
	Transform B2W = A2W * B2A;
	draw_cylinder(B2W*scale);
	draw_sphere(B2W*scaleS);
	draw_axe(B2W*scaleA);

	Transform scaleP = Scale(5, 50, 5);
	Transform C2B = Translation(0, 100, 0) * RotationZ(c);
	Transform C2W = B2W * C2B;
	draw_cylinder(C2W*scaleP);
	draw_sphere(C2W*scaleS);
	draw_axe(C2W*scaleA);

	Transform D2B = Translation(0, 100, 0) * RotationZ(d);
	Transform D2W = B2W * D2B;
	draw_cylinder(D2W*scaleP);
	draw_axe(D2W*scaleA);
	draw_sphere(D2W*scaleS);
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



