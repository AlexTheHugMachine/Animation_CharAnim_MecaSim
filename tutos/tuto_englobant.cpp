
//! \file tuto_englobant.cpp

#include <algorithm>
#include <vector>
#include <cfloat>
#include <chrono>

#include "vec.h"
#include "mat.h"
#include "color.h"
#include "image.h"
#include "image_io.h"
#include "image_hdr.h"
#include "orbiter.h"
#include "mesh.h"
#include "wavefront.h"


struct RayHit
{
    Point o;            // origine
    float t;            // p(t)= o + td, position du point d'intersection sur le rayon
    Vector d;           // direction
    int triangle_id;    // indice du triangle dans le mesh
    float u, v;
    int x, y;
    
    RayHit( const Point& _o, const Point& _e ) :  o(_o), t(1), d(Vector(_o, _e)), triangle_id(-1), u(), v(), x(), y() {}
    RayHit( const Point& _o, const Point& _e, const int _x, const int _y ) :  o(_o), t(1), d(Vector(_o, _e)), triangle_id(-1), u(), v(), x(_x), y(_y) {}
    operator bool ( ) { return (triangle_id != -1); }
};


struct Triangle
{
    Point p;            // sommet a du triangle
    Vector e1, e2;      // aretes ab, ac du triangle
    int id;
    
    Triangle( const TriangleData& data, const int _id ) : p(data.a), e1(Vector(data.a, data.b)), e2(Vector(data.a, data.c)), id(_id) {}
    
    /* calcule l'intersection ray/triangle
        cf "fast, minimum storage ray-triangle intersection" 
        
        renvoie faux s'il n'y a pas d'intersection valide (une intersection peut exister mais peut ne pas se trouver dans l'intervalle [0 tmax] du rayon.)
        renvoie vrai + les coordonnees barycentriques (u, v) du point d'intersection + sa position le long du rayon (t).
        convention barycentrique : p(u, v)= (1 - u - v) * a + u * b + v * c
    */
    void intersect( RayHit &ray ) const
    {
        Vector pvec= cross(ray.d, e2);
        float det= dot(e1, pvec);
        
        float inv_det= 1 / det;
        Vector tvec(p, ray.o);
        
        float u= dot(tvec, pvec) * inv_det;
        if(u < 0 || u > 1) return;
        
        Vector qvec= cross(tvec, e1);
        float v= dot(ray.d, qvec) * inv_det;
        if(v < 0 || u + v > 1) return;
        
        float t= dot(e2, qvec) * inv_det;
        if(t < 0 || t > ray.t) return;
        
        // touche !!
        ray.t= t;
        ray.triangle_id= id;
        ray.u= u;
        ray.v= v;
    }
};



struct BBox
{
    Point pmin, pmax;
    
    BBox( ) : pmin(), pmax() {}
    
    BBox( const Point& p ) : pmin(p), pmax(p) {}
    BBox& insert( const Point& p ) { pmin= min(pmin, p); pmax= max(pmax, p); return *this; }
    
    float centroid( const int axis ) const { return (pmin(axis) + pmax(axis)) / 2; }
    
    bool intersect( const RayHit& ray ) const
    {
        Vector invd= Vector(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
        return intersect(ray, invd);
    }
    
    bool intersect( const RayHit& ray, const Vector& invd ) const
    {
        Point rmin= pmin;
        Point rmax= pmax;
        if(ray.d.x < 0) std::swap(rmin.x, rmax.x);
        if(ray.d.y < 0) std::swap(rmin.y, rmax.y);
        if(ray.d.z < 0) std::swap(rmin.z, rmax.z);
        Vector dmin= (rmin - ray.o) * invd;
        Vector dmax= (rmax - ray.o) * invd;
        
        float tmin= std::max(dmin.z, std::max(dmin.y, std::max(dmin.x, 0.f)));
        float tmax= std::min(dmax.z, std::min(dmax.y, std::min(dmax.x, ray.t)));
        return (tmin <= tmax);
    }
};


void direct( 
    const std::vector<Triangle>& triangles, const int tbegin, const int tend,
    std::vector<RayHit>& rays, const int rbegin, const int rend )
{
    for(int i= rbegin; i < rend; i++)
    for(int k= tbegin; k < tend; k++)
        triangles[k].intersect(rays[i]);
}

struct triangle_less1
{
    int axis;
    float cut;
    
    triangle_less1( const int _axis, const float _cut ) : axis(_axis), cut(_cut) {}
    
    bool operator() ( const Triangle& triangle ) const
    {
        // re-construit l'englobant du triangle
        BBox bounds(triangle.p);
        bounds.insert(triangle.p + triangle.e1);
        bounds.insert(triangle.p + triangle.e2);
        return bounds.centroid(axis) < cut;
    }
};

struct ray_less1
{
    BBox bounds;
    
    ray_less1( const BBox& _bounds ) : bounds(_bounds) {}
    
    bool operator() ( const RayHit& ray ) const
    {
        return bounds.intersect(ray);
    }
};


void divide( const BBox& bounds, 
    std::vector<Triangle>& triangles, const int tbegin, const int tend,
    std::vector<RayHit>& rays, const int rbegin, const int rend )
{
    if(tbegin == tend || rbegin == rend)
        // plus de triangles ou de rayons, rien a faire...
        return;
        
    // il ne reste plus que quelques triangles, finir les calculs d'intersection...
    if(tend - tbegin <= 4)
    {
        direct(triangles, tbegin, tend, rays, rbegin, rend);
        return;
    }
    
    // axe le plus etire de l'englobant
    Vector d= Vector(bounds.pmin, bounds.pmax);
    int axis;
    if(d.x > d.y && d.x > d.z)  // x plus grand que y et z ?
        axis= 0;
    else if(d.y > d.z)          // y plus grand que z ? (et que x implicitement)
        axis= 1;
    else                        // x et y ne sont pas les plus grands...
        axis= 2;

    // coupe l'englobant au milieu
    float cut= bounds.centroid(axis);
    
    // repartit les triangles 
    Triangle *pm= std::partition(triangles.data() + tbegin, triangles.data() + tend, triangle_less1(axis, cut));
    int m= std::distance(triangles.data(), pm);
    
    // la repartition des triangles peut echouer, et tous les triangles sont dans la meme partie... 
    // forcer quand meme un decoupage en 2 ensembles 
    if(m == tbegin || m == tend)
        m= (tbegin + tend) / 2;
    assert(m != tbegin);
    assert(m != tend);
    
    // construit les englobants
    BBox left(triangles[tbegin].p);
    left.insert(triangles[tbegin].p + triangles[tbegin].e1);
    left.insert(triangles[tbegin].p + triangles[tbegin].e2);
    for(int i= tbegin+1; i < m; i++)
    {
        left.insert(triangles[i].p);
        left.insert(triangles[i].p + triangles[i].e1);
        left.insert(triangles[i].p + triangles[i].e2);
    }
    
    // repartit les rayons
    RayHit *prleft= std::partition(rays.data() + rbegin, rays.data() + rend, ray_less1(left));
    int rleft= std::distance(rays.data(), prleft);
    
    divide(left, triangles, tbegin, m, rays, rbegin, rleft);
    
    // on recommence pour la droite
    BBox right(triangles[m].p);
    right.insert(triangles[m].p + triangles[m].e1);
    right.insert(triangles[m].p + triangles[m].e2);
    for(int i= m+1; i < tend; i++)
    {
        right.insert(triangles[i].p);
        right.insert(triangles[i].p + triangles[i].e1);
        right.insert(triangles[i].p + triangles[i].e2);
    }
    
    RayHit *prright= std::partition(rays.data() + rbegin, rays.data() + rend, ray_less1(right));
    int rright= std::distance(rays.data(), prright);
    
    divide(right, triangles, m, tend, rays, rbegin, rright);
}


int main( const int argc, const char **argv )
{
    const char *mesh_filename= "data/cornell.obj";
    if(argc > 1)
        mesh_filename= argv[1];
        
    const char *orbiter_filename= "data/cornell_orbiter.txt";
    if(argc > 2)
        orbiter_filename= argv[2];
    
    Orbiter camera;
    if(camera.read_orbiter(orbiter_filename) < 0)
        return 1;

    Mesh mesh= read_mesh(mesh_filename);
    BBox bounds;
    mesh.bounds(bounds.pmin, bounds.pmax);
    
    // recupere les triangles
    std::vector<Triangle> triangles;
    {
        int n= mesh.triangle_count();
        for(int i= 0; i < n; i++)
            triangles.emplace_back(mesh.triangle(i), i);
    }
    
    Image image(1024, 768);

    // recupere les transformations
    camera.projection(image.width(), image.height(), 45);
    Transform model= Identity();
    Transform view= camera.view();
    Transform projection= camera.projection();
    Transform viewport= camera.viewport();
    Transform inv= Inverse(viewport * projection * view * model);
    
    // genere un rayon par pixel de l'image
    std::vector<RayHit> rays;
    for(int y= 0; y < image.height(); y++)
    for(int x= 0; x < image.width(); x++)
    {
        // generer le rayon
        Point origine= inv(Point(x + .5f, y + .5f, 0));
        Point extremite= inv(Point(x + .5f, y + .5f, 1));
        
        rays.emplace_back(origine, extremite, x, y);
    }
    
// mesure les temps d'execution 
#if 0
    {
        auto start= std::chrono::high_resolution_clock::now();
        direct(triangles, 0, int(triangles.size()), rays, 0, int(rays.size()));
        
        auto stop= std::chrono::high_resolution_clock::now();
        int cpu= std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        printf("direct %dms\n", cpu);
    }
#endif

    {
        auto start= std::chrono::high_resolution_clock::now();
        divide(bounds, triangles, 0, int(triangles.size()), rays, 0, int(rays.size()));
        
        auto stop= std::chrono::high_resolution_clock::now();
        int cpu= std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        printf("divide %dms\n", cpu);
    }
    
    // reconstruit l'image
    for(int i= 0; i < rays.size(); i++)
    {
        if(rays[i])
        {
            int x= rays[i].x;
            int y= rays[i].y;
            float u= rays[i].u;
            float v= rays[i].v;
            float w= 1 - u - v;
            image(x, y)= Color(w, u, v);
        }
    }
    write_image(image, "divide.png");
    
    
    return 0;
}
