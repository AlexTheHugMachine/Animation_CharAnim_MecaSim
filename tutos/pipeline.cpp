
#include <cstdio>
#include <cmath>

#include "vec.h"
#include "mat.h"

#include "mesh.h"
#include "image.h"
#include "orbiter.h"

#include "wavefront.h"


struct ZBuffer
{
    std::vector<float> data;
    int width;
    int height;
    
    ZBuffer( const int w, const int h )
    {
        width= w;
        height= h;
        data.assign(w * h, 1);
    }
    
    void clear( const float value= 1 )
    {
        data.assign(width * height, value);
    }
    
    float& operator() ( const int x, const int y )
    {
        const unsigned long int offset= y * width + x;
        return data[offset];
    }
};


struct Fragment
{
    float x, y, z;  // coordonnees espace image
    float u, v, w;  // coordonnees barycentriques du fragment dans le triangle abc, p(u, v, w) = u * c + v * a + w * b;
};


// interface
struct Pipeline
{
    Pipeline( ) {}
    virtual ~Pipeline( ) {}
    
    // vertex shader, doit renvoyer les coordonnees du sommet dans le repere projectif
    virtual Point vertex_shader( const int vertex_id ) const = 0;
    
    // fragment shader, doit renvoyer la couleur du fragment de la primitive
    // doit interpoler lui meme les "varyings", fragment.uvw definissent les coefficients.
    virtual Color fragment_shader( const int primitive_id, const Fragment fragment ) const = 0;
    // pour simplifier le code, les varyings n'existent pas dans cette version,
    // il faut recuperer les infos des sommets de la primitive et faire l'interpolation.
    // remarque : les gpu amd gcn fonctionnent comme ca...
};

// pipeline simple
struct BasicPipeline : public Pipeline
{
    const Mesh& mesh;
    Transform model;
    Transform view;
    Transform projection;
    Transform mvp;
    Transform mv;
    
    BasicPipeline( const Mesh& _mesh, const Transform& _model, const Transform& _view, const Transform& _projection ) 
        : Pipeline(), mesh(_mesh), model(_model), view(_view), projection(_projection) 
    {
        mvp= projection * view * model;
        mv= make_normal_transform(view * model);
    }
    
    Point vertex_shader( const int vertex_id ) const
    {
        // recupere la position du sommet
        Point p= make_point( mesh.positions[vertex_id] );
        // renvoie les coordonnees dans le repere projectif
        return transform(mvp, p);
    }
    
    Color fragment_shader( const int primitive_id, const Fragment fragment ) const
    {
        // recuperer les normales des sommets de la primitive
        Vector a= transform(mv, make_vector( mesh.normals[primitive_id * 3] ));
        Vector b= transform(mv, make_vector( mesh.normals[primitive_id * 3 +1] ));
        Vector c= transform(mv, make_vector( mesh.normals[primitive_id * 3 +2] ));
        
        // interpoler la normale
        Vector n= fragment.u * c + fragment.v * a + fragment.w * b;
        // et la normaliser, l'interpolation ne conserve pas la longueur des vecteurs
        n= normalize(n);
        
        // calcule une couleur qui depend de l'orientation de la primitive par rapport a la camera
        return make_color(1, 0.5, 0) * std::abs(n.z);
        
        // on peut faire autre chose, par exemple, afficher la normale...
        // return make_color(std::abs(n.x), std::abs(n.y), std::abs(n.z));
    }
};


struct Edge
{
    float a, b, c;
    
    Edge( const Point& pa, const Point& pb )
    {
        a= -(pb.y - pa.y);
        b= pb.x - pa.x;
        c= -pa.x * a - pa.y * b;
    }
    
    // renvoie l'aire du triangle pa, pb, (x, y)
    float eval( const float x, const float y ) const { return (a*x + b*y + c) / 2; }
};

bool visible( const Point p )
{
    if(p.x < -1 || p.x > 1) return false;
    if(p.y < -1 || p.y > 1) return false;
    if(p.z < -1 || p.z > 1) return false;
    return true;
}


// juste pour linker avec window.cpp
int draw( ) { return 0; }

int main( void )
{
    Image color= create_image(1024, 640, 3, make_color(0, 0, 0));
    ZBuffer depth= ZBuffer(color.width, color.height);
    
    Mesh mesh= read_mesh("data/bigguy.obj");
    printf("  %u positions\n", (unsigned int) mesh.positions.size());
    printf("  %u indices\n", (unsigned int) mesh.indices.size());
    
    // regle le point de vue de la camera pour observer l'objet
    Point pmin, pmax;
    mesh_bounds(mesh, pmin, pmax);
    Orbiter camera= make_orbiter_lookat(center(pmin, pmax), distance(pmin, pmax));

    BasicPipeline pipeline( mesh, 
        make_identity(), 
        orbiter_view_transform(camera), 
        orbiter_projection_transform(camera, color.width, color.height, 45) );
    
    Transform viewport= make_viewport(color.width, color.height);
    
    // draw(pipeline, mesh.positions.size());
    for(unsigned int i= 0; i +2 < (unsigned int) mesh.positions.size(); i= i +3)
    {
        // transforme les 3 sommets du triangle
        Point a= pipeline.vertex_shader(i);
        Point b= pipeline.vertex_shader(i+1);
        Point c= pipeline.vertex_shader(i+2);
        
        // visibilite
        if(visible(a) == false && visible(b) == false && visible(c) == false)
            continue;
        
        // passage dans le repere image
        a= transform(viewport, a);
        b= transform(viewport, b);
        c= transform(viewport, c);
        
        // question: comment ne pas dessiner le triangle s'il est mal oriente ?
        
        // dessiner le triangle
        // solution naive, parcours tous les pixels de l'image
        // question : comment eviter de tester tous les pixels ? 
        // indice : il est sans doute possible de determiner que le triangle ne touche pas un bloc de pixels en ne testant que les coins...
        for(int y= 0; y < color.height; y++)
        for(int x= 0; x < color.width; x++)
        {
            // determine si le pixel est a l'interieur du triangle
            Edge ab(a ,b);
            Edge bc(b, c);
            Edge ca(c, a);
            
            // fragment 
            Fragment frag;
            frag.x= x;
            frag.y= y;
            frag.z= 0; // interpole plus tard
            
            frag.u= ab.eval(x, y);      // distance c / ab
            frag.v= bc.eval(x, y);      // distance a / bc
            frag.w= ca.eval(x, y);      // distance b / ac
            if(frag.u > 0 && frag.v > 0 && frag.w > 0)
            {
                // aire du triangle abc
                float area= length(cross( make_vector(a, b), make_vector(a, c) ));
                // normalise les coordonnees barycentriques du fragment
                frag.u= frag.u / area;
                frag.v= frag.v / area;
                frag.w= frag.w / area;
                // interpole z
                frag.z= frag.u * c.z + frag.v * a.z + frag.w * b.z;
                
                // evalue la couleur du fragment du triangle
                Color frag_color= pipeline.fragment_shader(i/3, frag);
                
                // ztest
                if(frag.z < depth(x, y))
                {
                    image_set_pixel(color, x, y, frag_color);
                    depth(x, y)= frag.z;
                }
                
                // question : pour quelle raison le ztest est-il fait apres l'execution du fragment shader ? est-ce obligatoire ?
                // question : peut on eviter d'executer le fragment shader sur un bloc de pixels couverts par le triangle ? 
                //   dans quelles conditions sait-on qu'il n'y a rien a dessiner dans un bloc de pixels ?
            }
        }
    }
    
    write_image(color, "render.png");
    return 0;
}
