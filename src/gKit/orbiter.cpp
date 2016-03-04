
#include "orbiter.h"


Orbiter make_orbiter( )
{
    Orbiter o;
    o.center= make_point(0, 0, 0);
    o.position= make_vec2(0, 0);
    o.rotation= make_vec2(0, 0);
    o.size= 5;
    return o;
}

Orbiter make_orbiter_lookat( const Point& center, const float size )
{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    Orbiter o;
    o.center= center;
    o.position= make_vec2(0, 0);
    o.rotation= make_vec2(0, 0);
    o.size= size;
    return o;
}

void orbiter_rotation( Orbiter&o, const float x, const float y )
{
    o.rotation.x= o.rotation.x + y;
    o.rotation.y= o.rotation.y + x;
}

void orbiter_translation( Orbiter&o, const float x, const float y )
{
    o.position.x= o.position.x - o.size * x;
    o.position.y= o.position.y + o.size * y;
}

void orbiter_move( Orbiter&o, const float z )
{
    o.size= o.size - o.size * 0.01f * z;
    if(o.size < 0.01f)
        o.size= 0.01f;
}

void orbiter_lookat( Orbiter&o, const Point& center, const float size )
{
    o= make_orbiter_lookat(center, size);
}

Transform orbiter_view_transform( const Orbiter&o )
{
    return make_translation( - make_vector(o.position.x, o.position.y, o.size) ) 
        * make_rotationX(o.rotation.x) * make_rotationY(o.rotation.y) 
        * make_translation( - make_vector(o.center) ); 
}

Transform orbiter_projection_transform( const Orbiter&o, const float width, const float height, const float fov )
{
    return make_perspective(fov, width / height, o.size*0.01f, o.size*2.f + o.center.z);
}

void orbiter_image_frame( const Orbiter& o, const float width, const float height, const float fov, Point& d0, Vector& dx, Vector& dy )
{
    Transform view= orbiter_view_transform(o);
    Transform projection= orbiter_projection_transform(o, width, height, fov);
    Transform viewport= make_viewport(width, height);
    Transform t= viewport * projection * view;  // passage monde vers image
    Transform tinv= make_inverse(t);            // l'inverse, passage image vers monde
    
    // origine du plan image
    d0= transform(tinv, make_point(0, 0, 0));
    // axe x du plan image
    Point d1= transform(tinv, make_point(1, 0, 0));
    // axe y du plan image
    Point d2= transform(tinv, make_point(0, 1, 0));
    
    dx= make_vector(d0, d1);
    dy= make_vector(d0, d2);
}

Point orbiter_position( const Orbiter& o )
{
    Transform t= orbiter_view_transform(o);     // passage monde vers camera
    Transform tinv= make_inverse(t);            // l'inverse, passage camera vers monde
    
    return transform(tinv, make_point(0, 0, 0));        // la camera se trouve a l'origine, dans le repere camera...
}
