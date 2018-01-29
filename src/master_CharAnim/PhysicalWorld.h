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

#ifndef WORLD_H
#define WORLD_H

#include <iostream>
#include <vector>

#include <Particle.h>
#include "vec.h"






class PhysicalWorld
{
public:
    PhysicalWorld(int n=0) : m_part(n) {}

    const Particle& operator[](int i) const { return m_part[i]; }
	Particle& operator[](int i)  { return m_part[i]; }

    std::size_t size() const { return m_part.size(); }
    void resize(int ns) { m_part.resize(ns); }

    void update(const float dt)
    {
        int i;
        for(i=0;i<m_part.size();++i)
        {
			//TODO
			// i_eme particule update
			// i_eme particule collision
			// i_eme particule add gravirty
        }
    }
protected:
    std::vector<Particle> m_part;
};


#endif // WORLD_H
