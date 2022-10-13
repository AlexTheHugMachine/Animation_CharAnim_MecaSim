
//! \file tuto_bvh2_gltf.cpp bvh 2 niveaux et instances, charge un fichier gltf...

#include <random>
#include <algorithm>
#include <vector>
#include <cfloat>

#include "vec.h"
#include "mat.h"
#include "color.h"
#include "image.h"
#include "image_io.h"
#include "orbiter.h"
#include "gltf.h"


// rayon 
struct Ray
{
    Point o;            // origine
    float pad;
    Vector d;           // direction
    float tmax;         // tmax= 1 ou \inf, le rayon est un segment ou une demi droite infinie
    
    Ray( const Point& _o, const Point& _e ) :  o(_o), d(Vector(_o, _e)), tmax(1) {} // segment, t entre 0 et 1
    Ray( const Point& _o, const Vector& _d ) :  o(_o), d(_d), tmax(FLT_MAX) {}  // demi droite, t entre 0 et \inf
    Ray( const Point& _o, const Vector& _d, const float _tmax ) :  o(_o), d(_d), tmax(_tmax) {} // explicite
};

// intersection avec un triangle
struct Hit
{
    float t;            // p(t)= o + td, position du point d'intersection sur le rayon
    float u, v;         // p(u, v), position du point d'intersection sur le triangle
    int instance_id;    // indice de l'instance, pour retrouver la transformation
    int mesh_id;        // indexation globale du triangle dans la scene gltf
    int primitive_id;
    int triangle_id;
    int pad;
    
    Hit( ) : t(FLT_MAX), u(), v(), instance_id(-1), mesh_id(-1), primitive_id(-1), triangle_id(-1) {}
    Hit( const Ray& ray ) : t(ray.tmax), u(), v(), instance_id(-1), mesh_id(-1), primitive_id(-1), triangle_id(-1) {}
    
    Hit( const float _t, const float _u, const float _v, const int _mesh_id, const int _primitive_id, const int _id ) : t(_t), u(_u), v(_v), 
        instance_id(-1), mesh_id(_mesh_id), primitive_id(_primitive_id), triangle_id(_id) {}
    
    operator bool ( ) { return (triangle_id != -1); }   // renvoie vrai si l'intersection est definie / existe
};

// intersection avec une boite / un englobant
struct BBoxHit
{
    float tmin, tmax;
    
    BBoxHit() : tmin(FLT_MAX), tmax(-FLT_MAX) {}
    BBoxHit( const float _tmin, const float _tmax ) : tmin(_tmin), tmax(_tmax) {}
    
    operator bool( ) const { return tmin <= tmax; }   // renvoie vrai si l'intersection est definie / existe
};


// boite englobante
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


// bvh parametre par le type des primitives, cf triangle et instance...
template < typename T >
struct BVHT
{
    // construit un bvh pour l'ensemble de primitives
    int build( const std::vector<T>& _primitives )
    {
        primitives= _primitives;  // copie les primitives pour les trier
        nodes.clear();          // efface les noeuds
        nodes.reserve(primitives.size());
        
        // construit l'arbre... 
        root= build(0, primitives.size());
        return root;
    }
    
    // intersection avec un rayon, entre 0 et htmax
    Hit intersect( const Ray& ray, const float htmax ) const
    {
        Hit hit;
        hit.t= htmax;
        Vector invd= Vector(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
        intersect(root, ray, invd, hit);
        return hit;
    }
    
    // intersection avec un rayon, entre 0 et ray.tmax
    Hit intersect( const Ray& ray ) const { return intersect(ray, ray.tmax); }
    
protected:
    std::vector<Node> nodes;
    std::vector<T> primitives;
    int root;
    
    int build( const int begin, const int end )
    {
        if(end - begin < 2)
        {
            // inserer une feuille et renvoyer son indice
            int index= nodes.size();
            nodes.push_back( make_leaf( primitive_bounds(begin, end), begin, end ) );
            return index;
        }
        
        // axe le plus etire de l'englobant des centres des englobants des primitives... 
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
        
        // repartit les primitives 
        T *pm= std::partition(primitives.data() + begin, primitives.data() + end, 
            [axis, cut]( const T& primitive ) 
            { 
                return primitive.bounds().centroid(axis) < cut; 
            }
        );
        int m= std::distance(primitives.data(), pm);
        
        // la repartition peut echouer, et toutes les primitives sont dans la meme moitiee de l'englobant
        // forcer quand meme un decoupage en 2 ensembles 
        if(m == begin || m == end)
            m= (begin + end) / 2;
        assert(m != begin);
        assert(m != end);
        
        // construire le fils gauche, les primtives se trouvent dans [begin .. m)
        int left= build(begin, m);
        
        // on recommence pour le fils droit, les primtives se trouvent dans [m .. end)
        int right= build(m, end);
        
        // construire le noeud et renvoyer son indice
        int index= nodes.size();
        nodes.push_back( make_node( BBox(nodes[left].bounds, nodes[right].bounds), left, right ) );
        return index;
    }
    
    // englobant des primitives
    BBox primitive_bounds( const int begin, const int end )
    {
        BBox bbox= primitives[begin].bounds();
        for(int i= begin +1; i < end; i++)
            bbox.insert(primitives[i].bounds());
        
        return bbox;
    }
    
    // englobant des centres des primitives
    BBox centroid_bounds( const int begin, const int end )
    {
        BBox bbox= primitives[begin].bounds().centroid();
        for(int i= begin +1; i < end; i++)
            bbox.insert(primitives[i].bounds().centroid());
        
        return bbox;
    }
    
    // intersection et parcours simple
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


// triangle pour le bvh, cf fonction bounds() et intersect()
struct Triangle
{
    Point p;            // sommet a du triangle
    Vector e1, e2;      // aretes ab, ac du triangle
    int mesh_id;
    int primitive_id;
    int triangle_id;
    
    Triangle( const vec3& a, const vec3& b, const vec3& c, const int _mesh_id, const int _primitive_id, const int _id ) : 
        p(a), e1(Vector(a, b)), e2(Vector(a, c)), 
        mesh_id(_mesh_id), primitive_id(_primitive_id), triangle_id(_id) {}
    
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
        
        return Hit(t, u, v, mesh_id, primitive_id, triangle_id);
    }
    
    BBox bounds( ) const 
    {
        BBox box(p);
        return box.insert(p+e1).insert(p+e2);
    }
};

typedef BVHT<Triangle> BVH;
typedef BVHT<Triangle> BLAS;


// instance pour le bvh, cf fonctions bounds() et intersect()
struct Instance
{
    Transform object_transform;
    BBox world_bounds;
    BVH *object_bvh;
    int instance_id;
    
    Instance( const BBox& bounds, const Transform& model, BVH *bvh, const int id ) :  
        object_transform(Inverse(model)), world_bounds(transform(bounds, model)), 
        object_bvh(bvh),
        instance_id(id)
    {}
    
    BBox bounds( ) const { return world_bounds; }
    
    Hit intersect( const Ray &ray, const float htmax ) const
    {
        // transforme le rayon
        Ray object_ray(object_transform(ray.o), object_transform(ray.d), htmax);
        // et intersection dans le bvh de l'objet instancie...
        
        Hit hit= object_bvh->intersect(object_ray, htmax);
        if(hit)
            // si intersection, stocker aussi l'indice de l'instance, cf retrouver la transformation et la matiere associee au mesh/triangle...
            hit.instance_id= instance_id;
        
        return hit;
    }
    
protected:
    BBox transform( const BBox& bbox, const Transform& m )
    {
        BBox bounds= BBox( m(bbox.pmin) );
        // enumere les sommets de la bbox 
        for(unsigned i= 1; i < 8; i++)
        {
            // chaque sommet de la bbox est soit pmin soit pmax sur chaque axe...
            Point p= bbox.pmin; 
            if(i & 1) p.x= bbox.pmax.x;
            if(i & 2) p.y= bbox.pmax.y;
            if(i & 4) p.z= bbox.pmax.z;
            
            // transforme le sommet de l'englobant 
            bounds.insert( m(p) );
        }
        
        return bounds;
    }
};

typedef BVHT<Instance> TLAS;


struct Sampler
{
    std::uniform_real_distribution<float> u01;
    std::default_random_engine rng;
    
    Sampler( const unsigned _seed ) : u01(), rng(_seed) {}
    void seed( const unsigned _seed ) { rng= std::default_random_engine(_seed); }
    
    float sample( ) { return u01(rng); }
    
    int sample_range( const int n ) { return int(sample() * n); }
};


int main( int argc, char **argv )
{
    const char *mesh_filename= "data/robot.gltf";
    const char *orbiter_filename= nullptr;
    
    if(argc > 1) mesh_filename= argv[1];
    if(argc > 2) orbiter_filename= argv[2];
    
    GLTFScene scene= read_gltf_scene(mesh_filename);
    
    // construit les bvh des objets de la scene, en parallele ! cf BLAS / bvh de triangles
    std::vector<BVH *> bvhs(scene.meshes.size());
    {
        // parcourir les mesh
        printf("%d meshes\n", int(scene.meshes.size()));
        
    #pragma omp parallel for
        for(unsigned mesh_id= 0; mesh_id < scene.meshes.size(); mesh_id++)
        {
            const GLTFMesh& mesh= scene.meshes[mesh_id];
            
            // groupes de triangles du mesh
            std::vector<Triangle> triangles;
            for(unsigned primitive_id= 0; primitive_id < mesh.primitives.size(); primitive_id++)
            {
                const GLTFPrimitives& primitives= mesh.primitives[primitive_id];
                
                for(unsigned i= 0; i +2 < primitives.indices.size(); i+= 3)
                {
                    // extraire les positions des sommets du triangle
                    vec3 a= primitives.positions[primitives.indices[i]];
                    vec3 b= primitives.positions[primitives.indices[i+1]];
                    vec3 c= primitives.positions[primitives.indices[i+2]];
                    triangles.push_back( Triangle(a, b, c, mesh_id, primitive_id, i/3) );
                    // stocke aussi l'indice du triangle
                }
            }
            
            BVH *bvh= new BVH;
            bvh->build(triangles);
            bvhs[mesh_id]= bvh;
        }
    }
    
    // instancie les objets de la scene, cf TLAS / bvh d'instances
    TLAS top_bvh;
    {
        printf("%d nodes\n", int(scene.nodes.size()));
        
        // 1 instance par noeud de la scene gltf
        std::vector<Instance> instances;
        for(unsigned node_id= 0; node_id < scene.nodes.size(); node_id++)
        {
            const GLTFNode& node= scene.nodes[node_id];
            const GLTFMesh& mesh= scene.meshes[node.mesh_index];
            
            instances.push_back( Instance( BBox(mesh.pmin, mesh.pmax), node.model, bvhs[node.mesh_index], node_id ) );
        }
        
        top_bvh.build(instances);
        printf("done. %d instances\n", int(instances.size()));
    }
    
    // recupere les matrices de la camera gltf
    assert(scene.cameras.size());
    Transform view= scene.cameras[0].view;
    Transform projection= scene.cameras[0].projection;
    
    // cree l'image en respectant les proportions largeur/hauteur de la camera gltf
    int width= 1024;
    int height= width / scene.cameras[0].aspect;
    Image image(width, height);
    
    // transformations
    Transform model= Identity();
    Transform viewport= Viewport(image.width(), image.height());
    Transform inv= Inverse(viewport * projection * view * model);
    
    
    // calcule l'image en parallele avec openMP
#pragma omp parallel for
    for(int y= 0; y < image.height(); y++)
    for(int x= 0; x < image.width(); x++)
    {
        // genere le rayon pour le pixel x,y
        Point o= inv( Point(x, y, 0) ); // origine
        Point e= inv( Point(x, y, 1) ); // extremite
        Ray ray(o, Vector(o, e));
        
        // intersections !
        if(Hit hit= top_bvh.intersect(ray))
        {
            assert(hit.instance_id != -1);
            assert(hit.mesh_id != -1);
            assert(hit.primitive_id != -1);
            // retrouve le triangle dans le mesh et ses primitives
            const GLTFMesh& mesh= scene.meshes[hit.mesh_id];
            const GLTFPrimitives& primitives= mesh.primitives[hit.primitive_id];
            // indices des sommets du triangle
            unsigned a= primitives.indices[3*hit.triangle_id];
            unsigned b= primitives.indices[3*hit.triangle_id+1];
            unsigned c= primitives.indices[3*hit.triangle_id+2];
            // c'est a ca que servent les indices ajoutes dans triangle, instance et hit !!
            
            /* exemple : normale interpolee du point d'intersection
                assert(primitives.normals.size());
                Vector na= primitives.normals[a];
                Vector nb= primitives.normals[b];
                Vector nc= primitives.normals[c];
                
                // normale interpolee au point d'intersection, cf coordonnees barycentriques dans le triangle
                Vector n= (1 - hit.u - hit.v) * na + hit.u * nb + hit.v * nc;
                // n est dans le repere local de l'objet... 
                
                // changement de repere vers la scene !
                const Transform& model= scene.nodes[hit.instance_id].model;   // recupere la transformation de l'instance...
                
                // todo : ecrire une fonction utilitaire !!
             */ 
             
            // retrouve la couleur de base du triangle
            Color diffuse= Yellow();
            if(primitives.material_index != -1)
            {
                const GLTFMaterial& material= scene.materials[primitives.material_index];
                
                // parametres du modele pbr / gltf : couleur / metal / rugosite
                Color base= material.color;
                diffuse= base;
            }
            
            image(x, y)= Color(diffuse, 1);
        }
    }
    printf("\n");
    
    write_image(image, "render.png");
    return 0;
}
