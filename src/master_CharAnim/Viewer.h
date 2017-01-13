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

#ifndef VIEWER_H
#define VIEWER_H

#include <ViewerBasic.h>

class Viewer : public ViewerBasic
{
public:
	Viewer() : ViewerBasic() {}

    int init();
    int render();
    int update( const float time, const float delta );

protected:

    Mesh m_cylinder;
    Mesh m_cylinder_cover;
    Mesh m_cone;
    Mesh m_sphere;

    void init_cone();
    void init_cylinder();
    void init_sphere();

    void draw_cylinder(const Transform& T);
    void draw_cone(const Transform& T);
	void draw_sphere(const Transform& T);
	void draw_cylinder(const Point& a, const Point& b, float r=1.f);
	void draw_sphere(const Point& a, float r = 1.f);
};


#endif
