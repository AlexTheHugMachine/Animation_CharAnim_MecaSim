
//! \file tuto_rayons.cpp

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


struct Ray
{
    Point o;            // origine
    Vector d;           // direction
    
    Ray( const Point& _o, const Point& _e ) :  o(_o), d(Vector(_o, _e)) {}
};

struct Hit
{
    float t;            // p(t)= o + td, position du point d'intersection sur le rayon
    float u, v;         // p(u, v), position du point d'intersection sur le triangle
    int triangle_id;    // indice du triangle dans le mesh
    
    Hit( ) : t(FLT_MAX), u(), v(), triangle_id(-1) {}
    Hit( const float _t, const float _u, const float _v, const int _id ) : t(_t), u(_u), v(_v), triangle_id(_id) {}
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
    Hit intersect( const Ray &ray, const float tmax ) const
    {
        Vector pvec= cross(ray.d, e2);
        float det= dot(e1, pvec);
        
        float inv_det= 1 / det;
        Vector tvec(p, ray.o);
        
        float u= dot(tvec, pvec) * inv_det;
        if(u < 0 || u > 1) return Hit();
        
        Vector qvec= cross(tvec, e1);
        float v= dot(ray.d, qvec) * inv_det;
        if(v < 0 || u + v > 1) return Hit();
        
        float t= dot(e2, qvec) * inv_det;
        if(t > tmax || t < 0) return Hit();
        
        return Hit(t, u, v, id);           // p(u, v)= (1 - u - v) * a + u * b + v * c
    }
};

Vector normal( const Mesh& mesh, const Hit& hit )
{
    // recuperer le triangle complet dans le mesh
    const TriangleData& data= mesh.triangle(hit.triangle_id);
    // interpoler la normale avec les coordonnées barycentriques du point d'intersection
    float w= 1 - hit.u - hit.v;
    Vector n= w * Vector(data.na) + hit.u * Vector(data.nb) + hit.v * Vector(data.nc);
    return normalize(n);
}

Color diffuse_color( const Mesh& mesh, const Hit& hit )
{
    const Material& material= mesh.triangle_material(hit.triangle_id);
    return material.diffuse;
}

struct Source
{
    Point s;
    Color emission;
};

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
    
    // recupere les triangles
    std::vector<Triangle> triangles;
    {
        int n= mesh.triangle_count();
        for(int i= 0; i < n; i++)
            triangles.emplace_back(mesh.triangle(i), i);
    }
    
    // recupere les sources
    std::vector<Source> sources;
    {
        int n= mesh.triangle_count();
        for(int i= 0; i < n; i++)
        {
            const Material& material= mesh.triangle_material(i);
            if(material.emission.r + material.emission.g + material.emission.b > 0)
            {
                // utiliser le centre du triangle comme source de lumière
                const TriangleData& data= mesh.triangle(i);
                Point p= (Point(data.a) + Point(data.b) + Point(data.c)) / 3;
                
                sources.push_back( { p, material.emission } );
            }
        }
        
        printf("%d sources\n", int(sources.size()));
        assert(sources.size() > 0);
    }
    
    Image image(1024, 768);

    // recupere les transformations
    camera.projection(image.width(), image.height(), 45);
    Transform model= Identity();
    Transform view= camera.view();
    Transform projection= camera.projection();
    Transform viewport= camera.viewport();
    Transform inv= Inverse(viewport * projection * view * model);
    
auto start= std::chrono::high_resolution_clock::now();
    
    // c'est parti, parcours tous les pixels de l'image
    for(int y= 0; y < image.height(); y++)
    for(int x= 0; x < image.width(); x++)
    {
        // generer le rayon
        Point origine= inv(Point(x + .5f, y + .5f, 0));
        Point extremite= inv(Point(x + .5f, y + .5f, 1));
        Ray ray(origine, extremite);
        
        // calculer les intersections avec tous les triangles
        Hit hit;
        for(int i= 0; i < int(triangles.size()); i++)
        {
            if(Hit h= triangles[i].intersect(ray, hit.t))
                // ne conserve que l'intersection la plus proche de l'origine du rayon
                hit= h;
        }
        
    #if 0
        if(hit)
            // coordonnees barycentriques de l'intersection
            image(x, y)= Color(1 - hit.u - hit.v, hit.u, hit.v);
    #endif

    #if 1
        if(hit)
        {
            Vector n= normal(mesh, hit);
            // normale interpolee a l'intersection
            image(x, y)= Color(std::abs(n.x), std::abs(n.y), std::abs(n.z));
        }
    #endif
    
    #if 0
        if(hit)
        {
            // position et emission de la source de lumiere
            Point s= sources[0].s;
            Color emission= sources[0].emission;
            
            // position du point d'intersection
            Point p= ray.o + hit.t * ray.d;
            // interpoler la normale au point d'intersection
            Vector pn= normal(mesh, hit);
            // direction de p vers la source s
            Vector l= Vector(p, s);
            
            // visibilite entre p et s
            float v= 1;
        #if 0
            Ray shadow_ray(p + 0.001f * pn, s);
            for(int i= 0; i < int(triangles.size()); i++)
            {
                if(triangles[i].intersect(shadow_ray, 1 - .001f))
                {
                    // on vient de trouver un triangle entre p et s. p est donc a l'ombre
                    v= 0;
                    break;  // pas la peine de continuer
                }
            }
        #endif
            
            // calculer la lumiere reflechie vers la camera / l'origine du rayon
            float cos_theta= std::abs(dot(pn, normalize(l)));
            Color fr= diffuse_color(mesh, hit) / M_PI;
            
            Color color= v * emission * fr * cos_theta / length2(l);
            image(x, y)= Color(color, 1);
        }
    #endif
    }
auto stop= std::chrono::high_resolution_clock::now();
int cpu= std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
printf("%dms\n", cpu);
    
    write_image(image, "render.png");
    write_image_hdr(image, "shadow.hdr");
    return 0;
}
