
//! \file tuto_bvh2.cpp bvh 2 niveaux et instances

#include <algorithm>
#include <vector>
#include <cfloat>

#include "vec.h"
#include "mat.h"
#include "color.h"
#include "image.h"
#include "image_io.h"
#include "orbiter.h"
#include "mesh.h"
#include "wavefront.h"


struct Ray
{
    Point o;            // origine
    float pad;
    Vector d;           // direction
    float tmax;
    
    Ray( const Point& _o, const Point& _e ) :  o(_o), d(Vector(_o, _e)), tmax(1) {}
    Ray( const Point& _o, const Vector& _d ) :  o(_o), d(_d), tmax(FLT_MAX) {}
    Ray( const Point& _o, const Vector& _d, const float _tmax ) :  o(_o), d(_d), tmax(_tmax) {}
};

struct Hit
{
    float t;            // p(t)= o + td, position du point d'intersection sur le rayon
    float u, v;         // p(u, v), position du point d'intersection sur le triangle
    int triangle_id;    // indice du triangle dans le mesh
    
    Hit( ) : t(FLT_MAX), u(), v(), triangle_id(-1) {}
    Hit( const Ray& ray ) : t(ray.tmax), u(), v(), triangle_id(-1) {}
    Hit( const float _t, const float _u, const float _v, const int _id ) : t(_t), u(_u), v(_v), triangle_id(_id) {}
    
    operator bool ( ) { return (triangle_id != -1); }
};

struct BBoxHit
{
    float tmin, tmax;
    
    BBoxHit() : tmin(FLT_MAX), tmax(-FLT_MAX) {}
    BBoxHit( const float _tmin, const float _tmax ) : tmin(_tmin), tmax(_tmax) {}
    
    operator bool( ) const { return tmin <= tmax; }
};


struct BBox
{
    Point pmin, pmax;
    
    BBox( ) : pmin(), pmax() {}
    
    BBox( const Point& p ) : pmin(p), pmax(p) {}
    BBox( const BBox& box ) : pmin(box.pmin), pmax(box.pmax) {}
    BBox( const BBox& a, const BBox& b ) : pmin(min(a.pmin, b.pmin)), pmax(max(a.pmax, b.pmax)) {}
    
    BBox& insert( const Point& p ) { pmin= min(pmin, p); pmax= max(pmax, p); return *this; }
    BBox& insert( const BBox& box ) { pmin= min(pmin, box.pmin); pmax= max(pmax, box.pmax); return *this; }
    
    float centroid( const int axis ) const { return (pmin(axis) + pmax(axis)) / 2; }
    Point centroid( ) const { return (pmin + pmax) / 2; }
    
    BBoxHit intersect( const Ray& ray, const float htmax ) const
    {
        Vector invd= Vector(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
        return intersect(ray, invd, htmax);
    }
    
    BBoxHit intersect( const Ray& ray, const Vector& invd, const float htmax ) const
    {
        Point rmin= pmin;
        Point rmax= pmax;
        if(ray.d.x < 0) std::swap(rmin.x, rmax.x);
        if(ray.d.y < 0) std::swap(rmin.y, rmax.y);
        if(ray.d.z < 0) std::swap(rmin.z, rmax.z);
        Vector dmin= (rmin - ray.o) * invd;
        Vector dmax= (rmax - ray.o) * invd;
        
        float tmin= std::max(dmin.z, std::max(dmin.y, std::max(dmin.x, 0.f)));
        float tmax= std::min(dmax.z, std::min(dmax.y, std::min(dmax.x, htmax)));
        return BBoxHit(tmin, tmax);
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


template < typename T >
struct BVHT
{
    std::vector<Node> nodes;
    std::vector<T> primitives;
    int root;
    
    // construit un bvh pour l'ensemble de triangles
    int build( const std::vector<T>& _primitives )
    {
        primitives= _primitives;  // copie les triangles pour les trier
        nodes.clear();          // efface les noeuds
        nodes.reserve(primitives.size());
        
        // construit l'arbre... 
        root= build(0, primitives.size());
        // et renvoie la racine
        return root;
    }
    
    Hit intersect( const Ray& ray ) const
    {
        Hit hit(ray);
        Vector invd= Vector(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
        intersect(root, ray, invd, hit);
        return hit;
    }
    
    Hit intersect( const Ray& ray, const float htmax ) const
    {
        Hit hit;
        hit.t= htmax;
        Vector invd= Vector(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
        intersect(root, ray, invd, hit);
        return hit;
    }
    
protected:
    int build( const int begin, const int end )
    {
        if(end - begin <= 2)
        {
            // inserer une feuille et renvoyer son indice
            int index= nodes.size();
            nodes.push_back( make_leaf( primitive_bounds(begin, end), begin, end ) );
            return index;
        }
        
        // axe le plus etire de l'englobant des centres 
        BBox cbounds= centroid_bounds(begin, end);
        Vector d= Vector(cbounds.pmin, cbounds.pmax);
        int axis;
        if(d.x > d.y && d.x > d.z)  // x plus grand que y et z ?
            axis= 0;
        else if(d.y > d.z)          // y plus grand que z ? (et que x implicitement)
            axis= 1;
        else                        // x et y ne sont pas les plus grands...
            axis= 2;

        // coupe l'englobant au milieu
        float cut= cbounds.centroid(axis);
        
        // repartit les triangles 
        T *pm= std::partition(primitives.data() + begin, primitives.data() + end, 
            [axis, cut]( const T& primitive ) 
            { 
                return primitive.bounds().centroid(axis) < cut; 
            }
        );
        int m= std::distance(primitives.data(), pm);
        
        // la repartition des triangles peut echouer, et tous les triangles sont dans la meme partie... 
        // forcer quand meme un decoupage en 2 ensembles 
        if(m == begin || m == end)
            m= (begin + end) / 2;
        assert(m != begin);
        assert(m != end);
        
        // construire le fils gauche, les triangles se trouvent dans [begin .. m)
        int left= build(begin, m);
        
        // on recommence pour le fils droit, les triangles se trouvent dans [m .. end)
        int right= build(m, end);
        
        // construire le noeud et renvoyer son indice
        int index= nodes.size();
        nodes.push_back( make_node( BBox(nodes[left].bounds, nodes[right].bounds), left, right ) );
        return index;
    }
    
    
    BBox primitive_bounds( const int begin, const int end )
    {
        BBox bbox= primitives[begin].bounds();
        for(int i= begin +1; i < end; i++)
            bbox.insert(primitives[i].bounds());
        
        return bbox;
    }
    
    BBox centroid_bounds( const int begin, const int end )
    {
        BBox bbox= primitives[begin].bounds().centroid();
        for(int i= begin +1; i < end; i++)
            bbox.insert(primitives[i].bounds().centroid());
        
        return bbox;
    }
    
    void intersect( const int index, const Ray& ray, const Vector& invd, Hit& hit ) const
    {
        const Node& node= nodes[index];
        if(node.bounds.intersect(ray, invd, hit.t))
        {
            if(node.leaf())
            {
                for(int i= node.leaf_begin(); i < node.leaf_end(); i++)
                    if(Hit h= primitives[i].intersect(ray, hit.t))
                        hit= h;
            }
            else // if(node.internal())
            {
                intersect(node.internal_left(), ray, invd, hit);
                intersect(node.internal_right(), ray, invd, hit);
            }
        }
    }
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
    Hit intersect( const Ray &ray, const float htmax ) const
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
        if(t < 0 || t > htmax) return Hit();
        
        return Hit(t, u, v, id);
    }
    
    BBox bounds( ) const 
    {
        BBox box(p);
        return box.insert(p+e1).insert(p+e2);
    }
};

typedef BVHT<Triangle> BVH;
typedef BVHT<Triangle> BLAS;

struct Instance
{
    Transform object_transform;
    BBox world_bounds;
    BVH *object_bvh;

    Instance( const BBox& bounds, const Transform& model, BVH& bvh ) :  object_transform(Inverse(model)), world_bounds(bounds), object_bvh(&bvh) {}
    
    BBox bounds( ) const { return world_bounds; }
    
    Hit intersect( const Ray &ray, const float htmax ) const
    {
        // transforme le rayon
        Ray object_ray(object_transform(ray.o), object_transform(ray.d), htmax);
        // et intersection dans le bvh de l'objet instancie...
        return object_bvh->intersect(object_ray, htmax);
    }
};

typedef BVHT<Instance> TLAS;


int main( int argc, char **argv )
{
    const char *mesh_filename= "data/robot.obj";
    const char *orbiter_filename= nullptr;
    
    if(argc > 1) mesh_filename= argv[1];
    if(argc > 2) orbiter_filename= argv[2];
    
    Mesh mesh= read_mesh(mesh_filename);
    if(mesh.triangle_count() == 0)
        return 1;
    
    BBox mesh_bounds;
    mesh.bounds(mesh_bounds.pmin, mesh_bounds.pmax);
    
    // construit le bvh de l'objet
    BVH bvh;
    {
        // construire l'ensemble de triangles du mesh
        std::vector<Triangle> primitives;
        for(int i= 0; i <mesh.triangle_count(); i++)
        {
            TriangleData data= mesh.triangle(i);
            primitives.push_back( Triangle(data, i) );
        }
        
        bvh.build(primitives);
    }
    
    // instancie l'objet
    TLAS top_bvh;
    {
        std::vector<Instance> primitives;
        for(int i= -2; i <= 2; i++)
        {
            Transform model= Translation(i*10, 0, 0);
            
            // calcule l'englobant de l'instance...
            BBox bounds;
            for(unsigned i= 0; i < 8; i++)
            {
                Point p= mesh_bounds.pmin;
                if(i & 1) p.x= mesh_bounds.pmax.x;
                if(i & 2) p.y= mesh_bounds.pmax.y;
                if(i & 4) p.z= mesh_bounds.pmax.z;
                
                // transforme le sommet de l'englobant 
                bounds.insert(model(p));
            }
            
            primitives.push_back( Instance(bounds, model, bvh) );
        }
        
        top_bvh.build(primitives);
    }
    
    Orbiter camera;
    if(orbiter_filename == nullptr || camera.read_orbiter(orbiter_filename) < 0)
        camera.lookat(mesh_bounds.pmin * 5, mesh_bounds.pmax * 5);      // les objets sont instancies sur une ligne...
        //~ camera.lookat(Origin(), 50);        // ou comme ca...

    Image image(1024, 640);
    
    Transform model= Identity();
    Transform view= camera.view();
    Transform projection= camera.projection();
    Transform viewport= Viewport(image.width(), image.height());
    Transform inv= Inverse(viewport * projection * view * model);
    
    // calcule l'image en parallele avec openMP
#pragma omp parallel for 
    for(int y= 0; y < image.height(); y++)
    for(int x= 0; x < image.width(); x++)
    {
        // genere le rayon pour le pixel x,y
        Point o= inv( Point(x, y, 0) );
        Point e= inv( Point(x, y, 1) );
        Ray ray(o, e);
        
        // intersections !
        if(Hit hit= top_bvh.intersect(ray))
            image(x, y)= Red(); // touche ! 
    }
    
    write_image(image, "render.png");
    return 0;
}
