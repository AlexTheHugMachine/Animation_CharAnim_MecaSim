
#include <cstdio>

#include "orbiter.h"

void Orbiter::lookat( const Point& center, const float size )
{
    m_center= center;
    m_position= vec2(0, 0);
    m_rotation= vec2(0, 0);
    m_size= size;
}

void Orbiter::lookat( const Point& pmin, const Point& pmax )
{
    lookat(center(pmin, pmax), distance(pmin, pmax));
}

void Orbiter::rotation( const float x, const float y )
{
    m_rotation.x= m_rotation.x + y;
    m_rotation.y= m_rotation.y + x;
}

void Orbiter::translation( const float x, const float y )
{
    m_position.x= m_position.x - m_size * x;
    m_position.y= m_position.y + m_size * y;
}

void Orbiter::move( const float z )
{
    m_size= m_size - m_size * 0.01f * z;
    if(m_size < 0.01f)
        m_size= 0.01f;
}

Transform Orbiter::view_transform( ) const
{
    return make_translation( - Vector(m_position.x, m_position.y, m_size) ) 
        * make_rotationX(m_rotation.x) * make_rotationY(m_rotation.y) 
        * make_translation( - Vector(m_center) ); 
}

Transform Orbiter::projection_transform( const float width, const float height, const float fov ) const
{
    return make_perspective(fov, width / height, m_size*0.01f, m_size*2.f + m_center.z);
}

void Orbiter::frame( const float width, const float height, const float z, const float fov, Point& dO, Vector& dx, Vector& dy ) const
{
    Transform view= view_transform();
    Transform projection= projection_transform(width, height, fov);
    Transform viewport= make_viewport(width, height);
    Transform t= viewport * projection * view;  // passage monde vers image
    Transform tinv= make_inverse(t);            // l'inverse, passage image vers monde
    
    // origine du plan image
    dO= transform(tinv, Point(0, 0, z));
    // axe x du plan image
    Point d1= transform(tinv, Point(1, 0, z));
    // axe y du plan image
    Point d2= transform(tinv, Point(0, 1, z));
    
    dx= Vector(dO, d1);
    dy= Vector(dO, d2);
}

Point Orbiter::position( )
{
    Transform t= view_transform();     // passage monde vers camera
    Transform tinv= make_inverse(t);            // l'inverse, passage camera vers monde
    
    return transform(tinv, Point(0, 0, 0));        // la camera se trouve a l'origine, dans le repere camera...
}

int Orbiter::read_orbiter( const char *filename )
{
    FILE *in= fopen(filename, "rt");
    if(in == NULL)
    {
        printf("[error] loading orbiter '%s'...\n", filename);
        return -1;
    }
    
    printf("loading orbiter '%s'...\n", filename);
    
    bool errors= false;
    if(fscanf(in, "c %f %f %f \n", &m_center.x, &m_center.y, &m_center.z) != 3)
        errors= true;
    if(fscanf(in, "p %f %f\n", &m_position.x, &m_position.y) != 2)
        errors= true;
    if(fscanf(in, "r %f %f\n", &m_rotation.x, &m_rotation.y) != 2)
        errors= true;
    if(fscanf(in, "s %f\n", &m_size) != 1)
        errors= true;
    
    fclose(in);
    if(errors)
    {
        printf("[error] loading orbiter '%s'...\n", filename);
        return -1;
    }
    
    return 0;
}

int Orbiter::write_orbiter( const char *filename )
{
    FILE *out= fopen(filename, "wt");
    if(out == NULL)
        return -1;
    
    printf("writing orbiter '%s'...\n", filename);
    
    fprintf(out, "c %f %f %f\n", m_center.x, m_center.y, m_center.z);
    fprintf(out, "p %f %f\n", m_position.x, m_position.y);
    fprintf(out, "r %f %f\n", m_rotation.x, m_rotation.y);
    fprintf(out, "s %f\n", m_size);
    
    fclose(out);
    return 0;
}
