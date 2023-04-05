
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

	/*controller.mc_bvh.resize(4);
	controller.mc_bvh[0].init( smart_path("data/bvh/motionGraph/null.bvh") );
	controller.mc_bvh[1].init( smart_path("data/bvh/motionGraph/marcher.bvh") );
	controller.mc_bvh[2].init( smart_path("data/bvh/motionGraph/courir.bvh") );
	controller.mc_bvh[3].init( smart_path("data/bvh/motionGraph/frapper.bvh") );*/
	//m_bvh.init( smart_path("data/bvh/motionGraph/marcher.bvh") );
    //m_bvh.init( smart_path("data/bvh/Robot.bvh") );
	//m_bvh.init( smart_path("data/bvh/danse.bvh") );

	controller.setMotionGraph(motionGraph);
	controller.setBvhMotionGraph();
	//controller.setBvhMotionGraph(m_bvh);

    m_frameNumber = 0;
	controller.setNbFrame(m_frameNumber);
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<controller.mc_bvh[0]<<endl;
    cout<<endl<<"========================"<<endl;

    m_ske.init( controller.mc_bvh[0] );
    m_ske.setPose( controller.mc_bvh[0], -1, controller, false);// met le skeleton a la pose au repos

    return 0;
}



void CharAnimViewer::draw_skeleton(const Skeleton& skl)
{
    // TODO Done

	for (int i = 0 ; i < skl.numberOfJoint() ; i++)
    {
        draw_sphere(skl.getJointPosition(i));
		if (skl.getParentId(i) != -1) {
			draw_cylinder(skl.getJointPosition(skl.getParentId(i)), skl.getJointPosition(i), 0.5f);
		}
    }

	/*const int N = 4;
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
	if (t >= N) t = 0.f;*/
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
	draw_sphere(controller.position(), 3.0f);
	draw_cylinder(controller.position(), controller.position() + controller.direction() * 10.0f, 1.0f);






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

	while(m_frameNumber!=time) {m_frameNumber++; }

	//controller.setVelocityMax(10.0f);
	if(key_state('z')) {
		controller.accelerate(0.01f * delta);
		if(controller.velocity() > 0.0f && controller.velocity() < 4.0f) {
			m_ske.setPose( controller.mc_bvh[1], m_frameNumber, controller, true );
			m_frameNumber = m_frameNumber%40;
		}
		else if(controller.velocity() >= 4.0f) {
			m_ske.setPose( controller.mc_bvh[2], m_frameNumber, controller, true );
			m_frameNumber = m_frameNumber%22;
		}
	}
    else
        controller.accelerate(-0.01f * delta);
		if(controller.velocity() > 0.0f && controller.velocity() < 4.0f) {
			m_ske.setPose( controller.mc_bvh[1], m_frameNumber, controller, true );
			m_frameNumber = m_frameNumber%40;
			/*for(int i = 0; i<40; i++) {
				m_frameNumber++;
			}
			m_frameNumber=0;*/
		}
		else if(controller.velocity() >= 4.0f) {
			m_ske.setPose( controller.mc_bvh[2], m_frameNumber, controller, true );
			m_frameNumber = m_frameNumber%22;
		}
		else if(controller.velocity() == 0.0f) {
			m_ske.setPose( controller.mc_bvh[0], m_frameNumber, controller, true );
			m_frameNumber = 0;
		}

    if(key_state('q'))
        controller.turnXZ(0.1f * delta);
    if(key_state('d'))
        controller.turnXZ(-0.1f * delta);

	if(key_state('x')) {
		m_ske.setPose( controller.mc_bvh[3], m_frameNumber, controller, true );
		m_frameNumber = m_frameNumber%289;
	}

	//controller.switchBetweenBVH(1);
	controller.update(delta);

	//m_frameNumber = m_frameNumber % 2;

	//m_ske.setPose( controller.mc_bvh[3], m_frameNumber, controller, true );

    m_world.update(0.1f);

    return 0;
}



