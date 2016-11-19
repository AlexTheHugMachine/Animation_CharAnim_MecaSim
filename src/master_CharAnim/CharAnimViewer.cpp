
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "CharAnimViewer.h"

using namespace std;


CharAnimViewer::CharAnimViewer() : Viewer()
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
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<m_bvh<<endl;
    cout<<endl<<"========================"<<endl;

    return 0;
}


int CharAnimViewer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Viewer::manageCameraLight();

    gl.camera(m_camera);


    Transform T = Translation( 1, 1, 1 )*m_T;
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

    m_T = RotationZ( degrees(0.01*time) );



    return 0;
}

