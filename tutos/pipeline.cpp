
#include <cstdio>
#include <cmath>

#include "vec.h"
#include "mat.h"

#include "mesh.h"
#include "image.h"
#include "image_io.h"
#include "orbiter.h"

#include "wavefront.h"


struct ZBuffer
{
    std::vector<float> data;
    int width;
    int height;
    
    ZBuffer( const int w, const int h, const float z= 1 ) : data(w*h, z), width(w), height(h) {}
    
    void clear( const float value= 1 ) { data.assign(width * height, value); }
    
    float& operator() ( const int x, const int y )
    {
        std::size_t offset= y * width + x;
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
        mv= Normal(view * model);
    }
    
    Point vertex_shader( const int vertex_id ) const
    {
        // recupere la position du sommet
        Point p= Point( mesh.positions().at(vertex_id) );
        // renvoie les coordonnees dans le repere projectif
        return mvp(p);
    }
    
    Color fragment_shader( const int primitive_id, const Fragment fragment ) const
    {
        // recuperer les normales des sommets de la primitive
        Vector a= mv( Vector( mesh.normals().at(primitive_id * 3) ));
        Vector b= mv( Vector( mesh.normals().at(primitive_id * 3 +1) ));
        Vector c= mv( Vector( mesh.normals().at(primitive_id * 3 +2) ));
        
        // interpoler la normale
        Vector n= fragment.u * c + fragment.v * a + fragment.w * b;
        // et la normaliser, l'interpolation ne conserve pas la longueur des vecteurs
        n= normalize(n);
        
        // calcule une couleur qui depend de l'orientation de la primitive par rapport a la camera
        return White() * std::abs(n.z);
        
        // on peut faire autre chose, par exemple, afficher directement la normale...
        // return Color(std::abs(n.x), std::abs(n.y), std::abs(n.z));
    }
};


// cf http://geomalgorithms.com/a01-_area.html, section modern triangles
float area( const Point p, const Point a, const Point b )
{
    Vector pa= Vector(p, a); pa.z= 0;
    Vector pb= Vector(p, b); pb.z= 0;
    return cross(pa, pb).z;
}


bool visible( const Point p )
{
    if(p.x < -1 || p.x > 1) return false;
    if(p.y < -1 || p.y > 1) return false;
    if(p.z < -1 || p.z > 1) return false;
    return true;
}


int main( int argc, char **argv )
{
    Image color(640, 320);
    ZBuffer depth(color.width(), color.height());
    
    Mesh mesh= read_mesh("data/bigguy.obj");
    if(mesh == Mesh::error())
        return 1;
    printf("  %d positions\n", mesh.vertex_count());
    printf("  %d indices\n", mesh.index_count());
    
    // regle le point de vue de la camera pour observer l'objet
    Point pmin, pmax;
    mesh.bounds(pmin, pmax);
    Orbiter camera(pmin, pmax);

    BasicPipeline pipeline( 
        mesh, 
        Identity(), 
        camera.view(), 
        camera.projection(color.width(), color.height(), 45) );
    
    Transform viewport= Viewport(color.width(), color.height());
    
    // draw(pipeline, mesh.vertex_count());
    for(unsigned int i= 0; i +2 < (unsigned int) mesh.vertex_count(); i= i +3)
    {
        // transforme les 3 sommets du triangle
        Point a= pipeline.vertex_shader(i);
        Point b= pipeline.vertex_shader(i+1);
        Point c= pipeline.vertex_shader(i+2);
        
        // visibilite
        if(visible(a) == false && visible(b) == false && visible(c) == false)
            continue;
        // faux dans pas mal de cas...
        // question : comment faire un test correct ?
        // indication : si tous les sommets sont du meme cote d'une face de la region observee par la camera, on est sur que le triangle n'est pas visible.
        // comment definir la region observee par la camera ? quelle est sa forme (dans quel repere) ? les coordonnees de ses sommets ?
        
        // passage dans le repere image
        a= viewport(a);
        b= viewport(b);
        c= viewport(c);
        
        // question: comment ne pas dessiner le triangle s'il est mal oriente ?
        // aire du triangle abc
        float n= area(a, b, c);
        if(n < 0)
            continue;
        
        // dessiner le triangle
        // solution naive, parcours tous les pixels de l'image
        // question : comment eviter de tester tous les pixels ? 
        // indice : il est sans doute possible de determiner que le triangle ne touche pas un bloc de pixels en ne testant que les 4 coins du bloc...
        // question : dessiner de tout petits triangles est tres long. comment gagner du temps en determinant qu'un triangle est suffisament petit/etire pour "passer" entre les pixels ?
        // (remarque: oui, ca arrive tout le temps...)
        for(int y= 0; y < color.height(); y++)
        for(int x= 0; x < color.width(); x++)
        {
            // fragment 
            Fragment frag;
            frag.u= area(Point(x, y, 0), a, b);      // distance c / ab
            frag.v= area(Point(x, y, 0), b, c);      // distance a / bc
            frag.w= area(Point(x, y, 0), c, a);      // distance b / ac
            
            if(frag.u > 0 && frag.v > 0 && frag.w > 0)
            {
                // normalise les coordonnees barycentriques du fragment
                frag.u= frag.u / n;
                frag.v= frag.v / n;
                frag.w= frag.w / n;
                
                frag.x= x;
                frag.y= y;
                // interpole z
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
                // question : peut on eviter d'executer le fragment shader sur un bloc de pixels couverts par le triangle ? 
                //      dans quelles conditions sait-on qu'il n'y a rien a dessiner dans un bloc de pixels ?
                //      == aucun fragment du triangle appartenant au bloc, ne peut modifier l'image et le zbuffer ?
            }
        }
    }
    
    write_image(color, "render.png");
    return 0;
}
