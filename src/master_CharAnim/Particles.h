/****************************************************************************
Copyright (C) 2010-2020 Alexandre Meyer

This file is part of Simea.

Simea is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Simea is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Simea.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef PARTICLES_H
#define PARTICLES_H

#include <iostream>
#include <vector>

#include "vec.h"



class Particle
{
public:

    Particle()
    {
        m_mass = 1.0;      // 1kg

		m_radius = 10 + rand() % 5;

        m_p.x = rand()%400 - 200;
        m_p.y = m_radius + 5 +rand()%100;
        m_p.z = rand()%400 - 200;

    }

    void update(const float dt=0.1f)		// advect
    {
        if (m_mass>0)
        {
            m_v = m_v + (dt/m_mass)*m_f;		 // mise à jour de la vitesse
            m_p = m_p + dt*m_v;                  // mise à jour de la position
            m_f = Vector(0,0,0);
        }
    }

	//! Collision with the ground
    void collision()
    {
		if (m_radius < 0) return;
        if (m_p.y < m_radius)
        {
            m_p.y = m_radius + (m_radius-m_p.y);
            m_v.y = -m_v.y;
            m_v = 0.99f * m_v;       // FRICTION = 0.8
        }
    }

	//! Collision with any other sphere of position p and radius 'radius'
	void collision(const Point& p, const float radius);
	//{
	//	if (m_radius < 0) return;
	//	if (... TODO
	//}

	//! Apply gravity
    void computeParticleForceGravityEarth()
    {
        m_f = m_f + Vector(0, -m_mass * 9.81f, 0);
    }

    const Point& position() const { return m_p;}
    float radius() const { return m_radius;}

    friend std::ostream& operator<<(std::ostream& o, const Particle& p)
    {
        o<<" p=("<<p.m_p.x<<","<<p.m_p.y<<") v=("<<p.m_v.x<<","<<p.m_p.y<<") m="<<p.m_mass<<std::endl;
        return o;
    }

protected:
    Point m_p;				//!< position
    float m_radius;			//!< radius
    Vector m_v;				//!< velocity m/s
    Vector m_f;				//!< force in N
    float m_mass;			//!< mass in kg
};



class Particles
{
public:
    Particles(int n=0) : m_part(n) {}

    const Particle& operator[](int i) const { return m_part[i]; }
	Particle& operator[](int i)  { return m_part[i]; }

    std::size_t size() const { return m_part.size(); }
    void resize(int ns) { m_part.resize(ns); }

    void update(const float dt)
    {
        int i;
        for(i=0;i<m_part.size();++i)
        {
            m_part[i].update(dt);
            m_part[i].collision();
            m_part[i].computeParticleForceGravityEarth();
        }
    }
protected:
    std::vector<Particle> m_part;
};


#endif // PARTICLES_H
