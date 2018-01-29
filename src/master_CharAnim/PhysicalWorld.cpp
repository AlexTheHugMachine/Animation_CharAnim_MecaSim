
#include <PhysicalWorld.h>

#include <CharAnimViewer.h>


void PhysicalWorld::update(const float dt)
{
	int i;
	for (i = 0; i<m_part.size(); ++i)
	{
		//TODO
		// i_eme particule update
		// i_eme particule collision
		// i_eme particule add gravirty
	}
}


void PhysicalWorld::draw()
{
	int i;
	for (i = 0; i<particlesCount(); ++i)
	{
		if (m_part[i].radius()>0)
			CharAnimViewer::singleton().draw_sphere(m_part[i].position(), m_part[i].radius());
	}
}
