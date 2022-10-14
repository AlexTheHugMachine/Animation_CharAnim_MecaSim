
//! \file tuto_ray_gltf.cpp

#include <vector>
#include <cfloat>
#include <chrono>

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

// intersection / gltf
struct Hit
{
    float t;            // p(t)= o + td, position du point d'intersection sur le rayon
    float u, v;         // p(u, v), position du point d'intersection sur le triangle
    int instance_id;    // indice de l'instance
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


// triangle / gltf
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
};


int main( const int argc, const char **argv )
{
    const char *filename= "cornell.gltf";
    if(argc > 1)
        filename= argv[1];
    
    // charge la scene
    GLTFScene scene= read_gltf_scene(filename);
    
    // recupere les triangles
    std::vector<Triangle> triangles;
    for(unsigned node_id= 0; node_id < scene.nodes.size(); node_id++)
    {
        const GLTFNode& node= scene.nodes[node_id];
        
        const Transform& model= node.model;
        int mesh_id= node.mesh_index;
        
        const GLTFMesh& mesh= scene.meshes[mesh_id];
        for(unsigned primitive_id= 0; primitive_id < mesh.primitives.size(); primitive_id++)
        {
            const GLTFPrimitives& primitives= mesh.primitives[primitive_id];
            for(unsigned i= 0; i +2 < primitives.indices.size(); i+= 3)
            {
                // indice des sommets
                int a= primitives.indices[i];
                int b= primitives.indices[i+1];
                int c= primitives.indices[i+2];
                
                // position des sommets
                Point pa= primitives.positions[a];
                Point pb= primitives.positions[b];
                Point pc= primitives.positions[c];
                
                // transforme les sommets dans le repere de la scene
                pa= model( Point(pa) );
                pb= model( Point(pb) );
                pc= model( Point(pc) );
                
                triangles.push_back( Triangle(pa, pb, pc, mesh_id, primitive_id, i/3) );
            }
        }
    }
    assert(triangles.size());
    
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
    
    // c'est parti, parcours tous les pixels de l'image
    for(int y= 0; y < image.height(); y++)
    for(int x= 0; x < image.width(); x++)
    {
        // generer le rayon
        Point origine= inv(Point(x, y, 0));
        Point extremite= inv(Point(x, y, 1));
        Ray ray(origine, extremite);
        
        // calculer les intersections avec tous les triangles
        Hit hit;
        for(unsigned i= 0; i < triangles.size(); i++)
        {
            if(Hit h= triangles[i].intersect(ray, hit.t))
                // ne conserve que l'intersection la plus proche de l'origine du rayon
                // attention !! h.t <= hit.t !! sinon ca ne marche pas...
                hit= h;
        }
        
        if(hit)
            // coordonnees barycentriques de l'intersection
            image(x, y)= Color(1 - hit.u - hit.v, hit.u, hit.v);
    }
    
    write_image(image, "render.png");
    return 0;
}
