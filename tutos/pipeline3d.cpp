
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
    Transform mvn;
    
    BasicPipeline( const Mesh& _mesh, const Transform& _model, const Transform& _view, const Transform& _projection ) 
        : Pipeline(), mesh(_mesh), model(_model), view(_view), projection(_projection) 
    {
        mvp= projection * view * model;
        mv= view * model;
        mvn= make_normal_transform(mv);
    }
    
    Point vertex_shader( const int vertex_id ) const
    {
        // recupere la position du sommet
        Point p= make_point( mesh.positions[vertex_id] );
        //~ // renvoie les coordonnees dans le repere projectif
        //~ return transform(mvp, p);
        
        // renvoie les coordonnees dans le repere camera
        return transform(mv, p);
    }
    
    Color fragment_shader( const int primitive_id, const Fragment fragment ) const
    {
        // recuperer les normales des sommets de la primitive
        Vector a= transform(mvn, make_vector( mesh.normals[primitive_id * 3] ));
        Vector b= transform(mvn, make_vector( mesh.normals[primitive_id * 3 +1] ));
        Vector c= transform(mvn, make_vector( mesh.normals[primitive_id * 3 +2] ));
        
        // interpoler la normale
        Vector n= fragment.u * c + fragment.v * a + fragment.w * b;
        // et la normaliser, l'interpolation ne conserve pas la longueur des vecteurs
        n= normalize(n);
        
        // calcule une couleur qui depend de l'orientation de la primitive par rapport a la camera
        Color color= make_color(1.0 - (primitive_id % 100) / 99.f, float(primitive_id % 10) / 9.f, float(primitive_id % 1000) / 999.f);
        return color * std::abs(n.z);
        
        // on peut faire autre chose, par exemple, afficher la normale...
        // return make_color(std::abs(n.x), std::abs(n.y), std::abs(n.z));
    }
};


//! cf http://geomalgorithms.com/a01-_area.html, modern triangles
float area( const Point p, const Point a, const Point b )
{
    Vector pa= make_vector(p, a); pa.z= 0;
    Vector pb= make_vector(p, b); pb.z= 0;
    return cross(pa, pb).z;
}

//! cf https://en.wikipedia.org/wiki/Tetrahedron#Volume
float volume( const Point p, const Point a, const Point b, const Point c )
{
    Vector ab= make_vector(a, b);
    Vector ac= make_vector(a, c);
    Vector ap= make_vector(a, p);
    
    return dot(ap, cross(ab, ac));
}

// juste pour linker avec window.cpp
int draw( ) { return 0; }


int main( int argc, char **argv )
{
    //~ Image color= create_image(1024, 640, 3, make_color(0.2, 0.2, 0.2));
    Image color(512, 320, Color(0.2, 0.2, 0.2));
    ZBuffer depth= ZBuffer(color.width, color.height);
    
    Mesh mesh= read_mesh("data/bigguy.obj");
    //~ Mesh mesh= read_mesh("data/cube.obj");
    if(mesh == Mesh::error())
        return 1;
    printf("  %u positions\n", (unsigned int) mesh.positions.size());
    printf("  %u indices\n", (unsigned int) mesh.indices.size());
    
    // regle le point de vue de la camera pour observer l'objet
    Point pmin, pmax;
    mesh_bounds(mesh, pmin, pmax);
    Orbiter camera= make_orbiter_lookat(center(pmin, pmax), distance(pmin, pmax));
    //~ Orbiter camera= make_orbiter_lookat(center(pmin, pmax), 3);

    Transform model= make_identity();
    Transform view= orbiter_view_transform(camera);
    Transform projection= orbiter_projection_transform(camera, color.width(), color.height(), 45);
    Transform viewport= make_viewport(color.width(), color.height());
    
    BasicPipeline pipeline(mesh, model, view, projection);
    
    // prepare fragmentation 3d, repere camera
    Transform t= viewport * projection;                 // passage camera vers image
    Transform tinv= make_inverse(t);                    // l'inverse, passage image vers camera
    
    Point d0= transform(tinv, make_point(0, 0, 0));     // origine du plan image, repere camera
    Point d1= transform(tinv, make_point(1, 0, 0));     // axe x du plan image
    Point d2= transform(tinv, make_point(0, 1, 0));     // axe y du plan image
    Vector dx= make_vector(d0, d1);
    Vector dy= make_vector(d0, d2);
    
    // draw(pipeline, mesh.positions.size());
    for(unsigned int i= 0; i +2 < (unsigned int) mesh.positions.size(); i= i +3)
    {
        // transforme les 3 sommets du triangle
        Point a= pipeline.vertex_shader(i);
        Point b= pipeline.vertex_shader(i+1);
        Point c= pipeline.vertex_shader(i+2);
        
        // visibilite
        //~ if(visible(a, b, c) == false)
            //~ continue;
        
        // passage dans le repere image
        //~ a= transform(viewport, a);
        //~ b= transform(viewport, b);
        //~ c= transform(viewport, c);
        
        // question: comment ne pas dessiner le triangle s'il est mal oriente ?

        // aire du triangle abc
        //~ float n= area(a, b, c);
        float n= volume(make_point(0, 0, 0), a, b, c);
        if(n < 0)
            continue;

        //~ // determine si le pixel est a l'interieur du triangle
        //~ Edge ab(a ,b);
        //~ Edge bc(b, c);
        //~ Edge ca(c, a);
        Point e= make_point(0, 0, 0);
        //~ Vector face_aeb == cross(make_vector(a, e), make_vector(a, b));
        //~ Vector face_bec == cross(make_vector(b, e), make_vector(b, c));
        //~ Vector face_cea == cross(make_vector(c, e), make_vector(c, a));
        //~ // rotation circulaire
        //~ Vector face_aeb == eba == cross(make_vector(e, b), make_vector(e, a));
        //~ Vector face_bec == ecb == cross(make_vector(e, c), make_vector(e, b));
        //~ Vector face_cea == eac == cross(make_vector(e, a), make_vector(e, c));
        Vector face_eba= cross(make_vector(b), make_vector(a));
        Vector face_ecb= cross(make_vector(c), make_vector(b));
        Vector face_eac= cross(make_vector(a), make_vector(c));
        
        // dessiner le triangle
        // solution naive, parcours tous les pixels de l'image
        // question : comment eviter de tester tous les pixels ? 
        // indice : il est sans doute possible de determiner que le triangle ne touche pas un bloc de pixels en ne testant que les coins...
        for(int y= 0; y < color.height(); y++)
        for(int x= 0; x < color.width(); x++)
        {
            // fragment 
            Fragment frag;
            
            //~ frag.u= area(make_point(x, y, 0), a, b);      // distance c / ab
            //~ frag.v= area(make_point(x, y, 0), b, c);      // distance a / bc
            //~ frag.w= area(make_point(x, y, 0), c, a);      // distance b / ca
            //~ frag.u= ab.eval(x, y);      // distance c / ab
            //~ frag.v= bc.eval(x, y);      // distance a / bc
            //~ frag.w= ca.eval(x, y);      // distance b / ca
            
            Point p= d0 + x*dx + y*dy;
            //~ frag.u= volume(p, a, e, b);      // volume p / face aeb == dot(ap, cross(ae, ab))
            //~ frag.v= volume(p, b, e, c);      // volume p / face bec == dot(bp, cross(be, bc))
            //~ frag.w= volume(p, c, e, a);      // volume p / face cea == dot(cp, cross(ce, ca))
            //~ // permutation circulaire, ne change pas l'orientation
            frag.u= dot(make_vector(p), face_eba);      // volume p / face eba == dot(ep, cross(eb, ea))
            frag.v= dot(make_vector(p), face_ecb);      // volume p / face ecb == dot(ep, cross(ec, eb))
            frag.w= dot(make_vector(p), face_eac);      // volume p / face eac == dot(ep, cross(ea, ec))
            // ww= volume(p, a, b, c);
            if(frag.u > 0 && frag.v > 0 && frag.w > 0)
            {
                // t tel que ww == 0, p est sur le plan du triangle abc
                float t= dot(make_vector(a), cross(make_vector(a, b), make_vector(a, c))) / dot(make_vector(p), cross(make_vector(a, b), make_vector(a, c)));
                frag.u= dot(t * make_vector(p), face_eba);
                frag.v= dot(t * make_vector(p), face_ecb);
                frag.w= dot(t * make_vector(p), face_eac);
                
                // normalise les coordonnees barycentriques du fragment
                float nabc= area(a, b, c);
                frag.u= frag.u / nabc;
                frag.v= frag.v / nabc;
                frag.w= frag.w / nabc;
                
                // interpole z
                frag.x= x;
                frag.y= y;
                frag.z= frag.u * c.z + frag.v * a.z + frag.w * b.z;
                
                // evalue la couleur du fragment du triangle
                Color frag_color= pipeline.fragment_shader(i/3, frag);
                
                // ztest
                if(frag.z < depth(x, y))
                {
                    color(x, y)= Color(frag_color, 1);
                    depth(x, y)= frag.z;
                }
                
                // question : pour quelle raison le ztest est-il fait apres l'execution du fragment shader ? est-ce obligatoire ?
                // question : peut on eviter d'executer le fragment shader sur un bloc de pixels entierement a l'interieur par le triangle ? 
                //   dans quelles conditions sait-on que le triangle ne sera pas visible pour les pixels d'un bloc ?
            }
        }
    }
    
    write_image(color, "render.png");
    return 0;
}
