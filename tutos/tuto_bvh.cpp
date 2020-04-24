
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



// construction de l'arbre / BVH
struct Node
{
    BBox bounds;
    int left;
    int right;
    
    bool internal( ) const { return right > 0; }                        // renvoie vrai si le noeud est un noeud interne
    int internal_left( ) const { assert(internal()); return left; }     // renvoie le fils gauche du noeud interne 
    int internal_right( ) const { assert(internal()); return right; }   // renvoie le fils droit
    
    bool leaf( ) const { return right < 0; }                            // renvoie vrai si le noeud est une feuille
    int leaf_begin( ) const { assert(leaf()); return -left; }           // renvoie le premier objet de la feuille
    int leaf_end( ) const { assert(leaf()); return -right; }            // renvoie le dernier objet
};

// creation d'un noeud interne
Node make_node( const BBox& bounds, const int left, const int right )
{
    Node node { bounds, left, right };
    assert(node.internal());    // verifie que c'est bien un noeud...
    return node;
}

// creation d'une feuille
Node make_leaf( const BBox& bounds, const int begin, const int end )
{
    Node node { bounds, -begin, -end };
    assert(node.leaf());        // verifie que c'est bien une feuille...
    return node;
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


struct BVH
{
    std::vector<Node> nodes;
    std::vector<Triangle> triangles;
    int root;
    
    int direct_tests;
    
    // construit un bvh pour l'ensemble de triangles
    int build( const BBox& _bounds, const std::vector<Triangle>& _triangles )
    {
        triangles= _triangles;  // copie les triangles pour les trier
        nodes.clear();          // efface les noeuds
        nodes.reserve(triangles.size());
        
        // construit l'arbre... 
        root= build(_bounds, 0, triangles.size());
        // et renvoie la racine
        return root;
    }
    
    void intersect( RayHit& ray ) const
    {
        intersect(root, ray);
    }
    
protected:
    // construction d'un noeud
    int build( const BBox& bounds, const int begin, const int end )
    {
        if(end - begin <= 2)
        {
            // inserer une feuille et renvoyer son indice
            int index= nodes.size();
            nodes.push_back(make_leaf(bounds, begin, end));
            return index;
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
        Triangle *pm= std::partition(triangles.data() + begin, triangles.data() + end, triangle_less1(axis, cut));
        int m= std::distance(triangles.data(), pm);
        
        // la repartition des triangles peut echouer, et tous les triangles sont dans la meme partie... 
        // forcer quand meme un decoupage en 2 ensembles 
        if(m == begin || m == end)
            m= (begin + end) / 2;
        assert(m != begin);
        assert(m != end);
        
        // construire le fils gauche
        // les triangles se trouvent dans [begin .. m)
        BBox bounds_left(triangles[begin].p);                                         // englobant du premier triangle
        bounds_left.insert(triangles[begin].p + triangles[begin].e1);                // on insere les 3 sommets...
        bounds_left.insert(triangles[begin].p + triangles[begin].e2);
        for(int i= begin+1; i < m; i++)                                        // insere les sommets des autres triangles
        {
            bounds_left.insert(triangles[i].p);
            bounds_left.insert(triangles[i].p + triangles[i].e1);
            bounds_left.insert(triangles[i].p + triangles[i].e2);
        }
        int left= build(bounds_left, begin, m);
        
        // on recommence pour le fils droit
        // les triangles se trouvent dans [m .. end)
        BBox bounds_right(triangles[m].p);
        bounds_right.insert(triangles[m].p + triangles[m].e1);
        bounds_right.insert(triangles[m].p + triangles[m].e2);
        for(int i= m+1; i < end; i++)
        {
            bounds_right.insert(triangles[i].p);
            bounds_right.insert(triangles[i].p + triangles[i].e1);
            bounds_right.insert(triangles[i].p + triangles[i].e2);
        }
        int right= build(bounds_right, m, end);
        
        int index= nodes.size();
        nodes.push_back(make_node(bounds, left, right));
        return index;
    }
    
    void intersect( const int index, RayHit& ray ) const
    {
        const Node& node= nodes[index];
        if(node.bounds.intersect(ray))
        {
            if(node.leaf())
            {
                for(int i= node.leaf_begin(); i < node.leaf_end(); i++)
                    triangles[i].intersect(ray);
            }
            else // if(node.internal())
            {
                intersect(node.internal_left(), ray);
                intersect(node.internal_right(), ray);
            }
        }
    }
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
    {
        BVH bvh;
        
        //~ auto start= std::chrono::high_resolution_clock::now();
        {
            auto start= std::chrono::high_resolution_clock::now();
            // construction 
            bvh.build(bounds, triangles);
            
            auto stop= std::chrono::high_resolution_clock::now();
            int cpu= std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
            printf("build %dms\n", cpu);
        }
        
        {
            auto start= std::chrono::high_resolution_clock::now();
            
            // intersection
            const int n= int(rays.size());
            for(int i= 0; i < n; i++)
                bvh.intersect(rays[i]);
            
            auto stop= std::chrono::high_resolution_clock::now();
            int cpu= std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
            printf("bvh %dms\n", cpu);
        }
        
        {
            auto start= std::chrono::high_resolution_clock::now();
            
            // intersection
            const int n= int(rays.size());
            #pragma omp parallel for schedule(dynamic, 1024)
            for(int i= 0; i < n; i++)
                bvh.intersect(rays[i]);
            
            auto stop= std::chrono::high_resolution_clock::now();
            int cpu= std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
            printf("bvh %dms\n", cpu);
        }
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
    write_image(image, "bvh.png");
    
    return 0;
}
