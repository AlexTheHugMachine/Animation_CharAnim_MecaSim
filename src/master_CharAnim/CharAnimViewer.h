
#ifndef _CHARANIMVIEWER_H
#define _CHARANIMVIEWER_H


#include "quaternion.h"
#include "Viewer.h"
#include "BVH.h"
#include "Skeleton.h"
#include "TransformQ.h"

#include <PhysicalWorld.h>

class CharAnimViewer : public Viewer
{
public:
    CharAnimViewer();

    int init();
    int render();
    int update( const float time, const float delta );


protected:

    chara::BVH m_bvh;
    int m_frameNumber;

    Skeleton m_ske;

    PhysicalWorld m_world;

	void draw_skeleton(const Skeleton& );
	void draw_particles();
};



#endif
