
#include <PhysicalWorld.h>

#include <CharAnimViewer.h>


void PhysicalWorld::update(const float dt)
{
	int i;
	for (i = 0; i<m_part.size(); ++i)
	{
		// i_eme particule update
		m_part[i].update(dt);
		// i_eme particule collision
		m_part[i].groundCollision();
		// i_eme particule add gravirty
		m_part[i].addEarthGravity();
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


void PhysicalWorld::collision(const Point& p, const float radius)
{
	int i;
	for (i = 0; i<m_part.size(); ++i)
	{
		m_part[i].collision(p,radius);
	}
}
