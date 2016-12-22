
#ifndef CHARANIMVIEWER_H
#define CHARANIMVIEWER_H

#include "quaternion.h"
#include "Viewer.h"
#include "BVH.h"
using namespace simea;

class CharAnimViewer : public Viewer
{
public:
    CharAnimViewer();

    int init();
    int render();
    int update( const float time, const float delta );


protected:

    BVH m_bvh;

    float m_angle_a;
    float m_angle_b;
    float m_angle_milieu_ab;

    Quaternion m_quat_a;
    Quaternion m_quat_b;
    Quaternion m_quat_milieu_ab;


    int m_frameNumber;
};



#endif
