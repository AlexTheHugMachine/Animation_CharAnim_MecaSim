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

#include <Viewer.h>
#include <cmath>

using namespace std;



void Viewer::init_sphere()
{
    const int divBeta = 26;
    const int divAlpha = divBeta/2;
    int i,j;
    float beta, alpha, alpha2;

    m_sphere = Mesh(GL_TRIANGLE_STRIP);

    m_sphere.color( Color(1, 1, 1) );

    for(i=0;i<divAlpha;++i)
    {
        alpha = -0.5f*M_PI + float(i)*M_PI/divAlpha;
        alpha2 = -0.5f*M_PI + float(i+1)*M_PI/divAlpha;

        for(j=0;j<divBeta;++j)
        {
            beta = float(j)*2.f*M_PI/(divBeta-1);

            m_sphere.texcoord(beta/(2.0f*M_PI), 0.5f+alpha/M_PI);
            m_sphere.normal( Vector(cos(alpha)*cos(beta),  sin(alpha), cos(alpha)*sin(beta)) );
            m_sphere.vertex( Point(cos(alpha)*cos(beta),  sin(alpha), cos(alpha)*sin(beta)) );


            m_sphere.texcoord(beta/(2.0f*M_PI), 0.5f+alpha2/M_PI);
            m_sphere.normal( Vector(cos(alpha2)*cos(beta),  sin(alpha2), cos(alpha2)*sin(beta)) );
            m_sphere.vertex( Point(cos(alpha2)*cos(beta),  sin(alpha2), cos(alpha2)*sin(beta))   );
        }

        m_sphere.restart_strip();
    }
}


void Viewer::init_cone()
{
    int i;
    const int div = 25;
    float alpha;
    float step = 2.0*M_PI / (div);

    m_cone.color( Color(1, 1, 1));

    m_cone = Mesh(GL_TRIANGLE_STRIP);

    for (i=0;i<=div;++i)
    {
        alpha = i*step;

        m_cone.normal(Vector( cos(alpha)/sqrtf(2.f),  1.f/sqrtf(2.f), sin(alpha)/sqrtf(2.f) ));

        m_cone.texcoord( float(i)/div, 0.f );
        m_cone.vertex( Point( cos(alpha),  0, sin(alpha) ));

        m_cone.texcoord(float(i)/div, 1.f );
        m_cone.vertex( Point(0,1,0) );

    }
}


void Viewer::draw_cone(const Transform& T)
{
	gl.lighting(false);
    gl.texture(0);
    gl.model( T );
    gl.draw( m_cone );
    gl.draw( m_cylinder_cover );

}

void Viewer::init_cylinder()
{
    int i;
    const int div = 25;
    float alpha;
    float step = 2.0*M_PI / (div);

    m_cylinder = Mesh(GL_TRIANGLE_STRIP);

    for (i=0;i<=div;++i)
    {
        alpha = i*step;
        m_cylinder.normal( Vector(cos(alpha),  0, sin(alpha)) );
        m_cylinder.texcoord(float(i)/div, 0.f );
        m_cylinder.vertex( Point(cos(alpha),  0, sin(alpha)));

        m_cylinder.normal( Vector(cos(alpha),  0, sin(alpha)) );
        m_cylinder.texcoord(float(i)/div, 1.f );
        m_cylinder.vertex( Point(cos(alpha),   1, sin(alpha)));
    }

    m_cylinder_cover = Mesh( GL_TRIANGLE_FAN );

    m_cylinder_cover.normal( Vector(0,1,0) );

    m_cylinder_cover.vertex( Point(0,0,0));
    for (i=0;i<=div;++i)
    {
        alpha = -i*step;
        m_cylinder_cover.vertex( Point(cos(alpha),  0, sin(alpha)));
    }
}


void Viewer::draw_cylinder(const Transform& T)
{
    gl.model( T );
    gl.draw( m_cylinder );

    Transform Tch = T * Translation( 0, 1, 0);
    gl.model( Tch );
    gl.draw( m_cylinder_cover );

    //Transform Tcb = T  * Translation( 0, -1, 0);
    Transform Tcb = T * Translation( 0, 0, 0) * Rotation( Vector(1,0,0), 180);
    gl.model( Tcb );
    gl.draw( m_cylinder_cover );
}


void Viewer::draw_cylinder(const Point& a, const Point& b, float r)
{
	Vector ab = b - a;
	Vector p,y,z;
	Vector abn = normalize(ab);
	float lab = length(ab);
	if (lab<0.00001f) return;
	if ( fabs(ab.x) > 0.25f)
		p = Vector(0, 1, 0);
	else
		p = Vector(1, 0, 0);

	y = cross(abn, p);
	y = normalize(y);
	z = cross(abn, y);
	Transform T(z, abn, y, Vector(0, 0, 0));
	//cout << T[0] << endl;
	//cout << T[1] << endl;
	//cout << T[2] << endl;
	//cout << T[3] << endl;

	draw_cylinder( Translation( Vector(a) ) * T * Scale(r, lab, r));
}

void Viewer::draw_sphere(const Point& a, float r)
{
	draw_sphere(Translation(Vector(a))*Scale(r, r, r));
}



void Viewer::draw_sphere(const Transform& T)
{
	gl.model(T);
	gl.draw(m_sphere);
}




int Viewer::render()
{
	ViewerBasic::render();

    return 1;
}


int Viewer::update( const float time, const float delta )
{
	cout << "Viewer::update" << endl;
    return 1;
}



int Viewer::init()
{
	ViewerBasic::init();

    init_cylinder();
    init_cone();
    init_sphere();

    return 1;
}
