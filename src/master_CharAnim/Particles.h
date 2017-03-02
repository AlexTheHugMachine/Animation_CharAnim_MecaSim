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
        m = 1.0;      // 1kg

		r = 3 + rand() % 5;

        p.x = rand()%400 - 200;
        p.y = r + 5 +rand()%100;
        p.z = rand()%400 - 200;

        r = 3+rand()%5;
    }

    void update(const float dt=0.1f)		// advect
    {
        //const float dt = 0.1;
        if (m>0)
        {
            v = v + (dt/m)*f;     // mise à jour de la vitesse
            p = p + dt*v;                   // mise à jour de la position
            f = Vector(0,0,0);
        }
    }

    void collision()
    {
        if (p.y < r)
        {
            p.y = r + (r-p.y);
            v.y = -v.y;
            v = 0.99f * v;       // FRICTION = 0.8
        }
    }


    void computeParticleForceGravityEarth()
    {
        f = f + Vector(0, -m * 9.81f, 0);
    }

    const Point& position() const { return p;}
    float radius() const { return r;}

    friend std::ostream& operator<<(std::ostream& o, const Particle& p)
    {
        o<<" p=("<<p.p.x<<","<<p.p.y<<") v=("<<p.v.x<<","<<p.p.y<<") m="<<p.m<<std::endl;
        return o;
    }

protected:
    Point p;         // position(p.x, p.y)
    float r;
    Vector v;         // vitesse en m/s (v.x,v.y)
    Vector f;         // force en N (f.x, f.y)
    float m;        // masse en kg
};



class Particles
{
public:
    Particles(int n=0) : m_part(n) {}

    const Particle& operator[](int i) const { return m_part[i]; }

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
