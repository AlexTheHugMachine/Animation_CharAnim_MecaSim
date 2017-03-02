
#ifndef _CHARANIMVIEWER_H
#define _CHARANIMVIEWER_H


#include "quaternion.h"
#include "Viewer.h"
#include "BVH.h"
//using namespace chara;		// oui oui c'est moche, à ne pas faire dans la vraie vie
#include "Skeleton.h"
#include "TransformQ.h"

#include <Particles.h>

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

    Particles m_part;

	void bvhDrawGL(const chara::BVH&, int frameNumber);
	void bvhDrawGLRec(const chara::BVHJoint&, int frameNumber, Transform& T); // la fonction récursive sur le squelette
	void draw_particles();
};



#endif
