
#include <cfloat>
#include <cmath>

#include "app.h"

#include "vec.h"
#include "color.h"
#include "mat.h"

#include "mesh.h"
#include "wavefront.h"
#include "draw.h"

#include "image.h"
#include "image_io.h"
#include "image_hdr.h"

#include "program.h"
#include "uniforms.h"
#include "texture.h"

#include "orbiter.h"

#define EPSILON 0.00001f


struct Ray
{
    Point o;
    Vector d;
    float tmax;
    
    Ray( const Point origine, const Point extremite ) : o(origine), d(Vector(origine, extremite)), tmax(1) {}
    Ray( const Point origine, const Vector direction ) : o(origine), d(direction), tmax(FLT_MAX) {}
    
    Point operator( ) ( const float t ) const { return o + t * d; }
};

struct Hit
{
    Point p;
    Vector n;
    float t, u, v;
    int object_id;
    
    Hit( ) : p(), n(), t(FLT_MAX), u(0), v(0), object_id(-1) {}
};

struct Triangle : public TriangleData
{
    Triangle( ) : TriangleData() {}
    Triangle( const TriangleData& data ) : TriangleData(data) {}
    
    /* calcule l'intersection ray/triangle
        cf "fast, minimum storage ray-triangle intersection" 
        http://www.graphics.cornell.edu/pubs/1997/MT97.pdf

        renvoie faux s'il n'y a pas d'intersection valide, une intersection peut exister mais peut ne pas se trouver dans l'intervalle [0 htmax] du rayon. \n
        renvoie vrai + les coordonnees barycentriques (ru, rv) du point d'intersection + sa position le long du rayon (rt). \n
        convention barycentrique : t(u, v)= (1 - u - v) * a + u * b + v * c \n
    */
    bool intersect( const Ray &ray, const float htmax, float &rt, float &ru, float&rv ) const
    {
        /* begin calculating determinant - also used to calculate U parameter */
        Vector ac= Vector(Point(a), Point(c));
        Vector pvec= cross(ray.d, ac);

        /* if determinant is near zero, ray lies in plane of triangle */
        Vector ab= Vector(Point(a), Point(b));
        float det= dot(ab, pvec);
        if(det > -EPSILON && det < EPSILON)
            return false;

        float inv_det= 1.0f / det;

        /* calculate distance from vert0 to ray origin */
        Vector tvec(Point(a), ray.o);

        /* calculate U parameter and test bounds */
        float u= dot(tvec, pvec) * inv_det;
        if(u < 0.0f || u > 1.0f)
            return false;

        /* prepare to test V parameter */
        Vector qvec= cross(tvec, ab);

        /* calculate V parameter and test bounds */
        float v= dot(ray.d, qvec) * inv_det;
        if(v < 0.0f || u + v > 1.0f)
            return false;

        /* calculate t, ray intersects triangle */
        rt= dot(ac, qvec) * inv_det;
        ru= u;
        rv= v;

        // ne renvoie vrai que si l'intersection est valide (comprise entre tmin et tmax du rayon)
        return (rt < htmax && rt > EPSILON);
    }

    //! renvoie un point a l'interieur du triangle connaissant ses coordonnees barycentriques.
    //! convention p(u, v)= (1 - u - v) * a + u * b + v * c
    Point point( const float u, const float v ) const
    {
        float w= 1.f - u - v;
        return Point(Vector(a) * w + Vector(b) * u + Vector(c) * v);
    }

    //! renvoie une normale a l'interieur du triangle connaissant ses coordonnees barycentriques.
    //! convention p(u, v)= (1 - u - v) * a + u * b + v * c
    Vector normal( const float u, const float v ) const
    {
        float w= 1.f - u - v;
        return Vector(na) * w + Vector(nb) * u + Vector(nc) * v;
    }
};


//
struct Source : public Triangle
{
    Color emission;
    
    Source( ) : Triangle(), emission() {}
    Source( const TriangleData& data, const Color& color ) : Triangle(data), emission(color) {}
};


// construit un repere ortho tbn, a partir d'un seul vecteur...
// cf "generating a consistently oriented tangent space" 
// http://people.compute.dtu.dk/jerf/papers/abstracts/onb.html
struct World
{
    World( const Vector& _n ) : n(_n) 
    {
        if(n.z < -0.9999999f)
        {
            t= Vector(0, -1, 0);
            b= Vector(-1, 0, 0);
        }
        else
        {
            float a= 1.f / (1.f + n.z);
            float d= -n.x * n.y * a;
            t= Vector(1.f - n.x * n.x * a, d, -n.x);
            b= Vector(d, 1.f - n.y * n.y * a, -n.y);
        }
    }
    
    Vector operator( ) ( const Vector& local )  const
    {
        return local.x * t + local.y * b + local.z * n;
    }
    
    Vector t;
    Vector b;
    Vector n;
};


GLuint make_texture( const int unit, const int width, const int height )
{
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0,
        GL_RGB32F, width, height, 0,
        GL_RGBA, GL_FLOAT, NULL);
    
    // fixe les parametres de filtrage par defaut
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glGenerateMipmap(GL_TEXTURE_2D);
    return texture;
}


struct IS : public App
{
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    IS( const char *filename ) : App(1024, 640) 
    {
        m_mesh= read_mesh(filename);
        if(m_mesh == Mesh::error())
            return;
        
        build_sources();
        build_triangles();
        
        if(m_camera.read_orbiter("orbiter.txt") < 0)
        {
            Point pmin, pmax;
            m_mesh.bounds(pmin, pmax);
            m_camera.lookat(pmin, pmax);
        }
    }
    
    int init( )
    {
        if(m_mesh == Mesh::error())
            return -1;
        
        m_ptexture= make_texture(0, window_width(), window_height());
        m_ntexture= make_texture(1, window_width(), window_height());
        m_vtexture= make_texture(2, window_width(), window_height());
        
        m_hitp= Image(window_width(), window_height());
        m_hitn= Image(window_width(), window_height());
        m_hitv= Image(window_width(), window_height());
        
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        m_program= read_program("tutos/M2/is.glsl");
        program_print_errors(m_program);
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest
        
        return 0;
    }
    
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(key_state('r'))
        {
            clear_key_state('r');
            reload_program(m_program, "tutos/M2/is.glsl");
            program_print_errors(m_program);
        }
        
        if(key_state('f'))
        {
            clear_key_state('f');
            Point pmin, pmax;
            m_mesh.bounds(pmin, pmax);
            m_camera.lookat(pmin, pmax);        
        }
        
        // deplace la camera
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.move(mx);
        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
        
        static int mode= 0;
        static vec2 texcoord= vec2(0, 0);
        if(key_state(' '))
        {
            clear_key_state(' ');
            mode= (mode +1) % 2;
        
            if(mode == 1)
            {
                int mx, my;
                SDL_GetMouseState(&mx, &my);
                vec2 pixel= vec2(mx, window_height() - my -1);
                texcoord= vec2(pixel.x / m_hitp.width(), pixel.y / m_hitp.height());
                printf("pixel %f %f\n", pixel.x, pixel.y);
                
                // go
                Point d0;
                Vector dx0, dy0;
                m_camera.frame(m_hitp.width(), m_hitp.height(), 0, 45, d0, dx0, dy0);
                
                Point d1;
                Vector dx1, dy1;
                m_camera.frame(m_hitp.width(), m_hitp.height(), 1, 45, d1, dx1, dy1);
                
                // pixel
                Point o= d0 + pixel.x*dx0 + pixel.y*dy0;
                Point e= d1 + pixel.x*dx1 + pixel.y*dy1;

                Point point;
                Vector normal;
                
                Ray ray(o, e);
                Hit hit;
                if(intersect(ray, hit))
                {
                    point= hit.p;
                    normal= hit.n;
                    
                    // frame
                #pragma omp parallel for schedule(dynamic, 16)
                    for(int y= 0; y < m_hitp.height(); y++)
                    for(int x= 0; x < m_hitp.width(); x++)
                    {
                        // clear
                        m_hitp(x, y)= Black();
                        m_hitn(x, y)= Black();
                        m_hitv(x, y)= Black();
                        
                        Point o= d0 + x*dx0 + y*dy0;
                        Point e= d1 + x*dx1 + y*dy1;
                        
                        Ray ray(o, e);
                        Hit hit;
                        if(intersect(ray, hit))
                        {
                            m_hitp(x, y)= Color(hit.p.x, hit.p.y, hit.p.z);
                            m_hitn(x, y)= Color(hit.n.x, hit.n.y, hit.n.z);
                            
                            Ray shadow(hit.p + hit.n * 0.001f, point + normal * 0.001f);
                            Hit shadow_hit;
                            int v= 1;
                            if(intersect(shadow, shadow_hit))
                                v= 0;
                            
                            m_hitv(x, y)= Color(v, v, v);
                        }
                    }
                    
                    // transferre les donnees
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, m_ptexture);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 
                        0, 0, m_hitp.width(), m_hitp.height(),
                        GL_RGBA, GL_FLOAT, m_hitp.buffer());
                    
                    glActiveTexture(GL_TEXTURE0 +1);
                    glBindTexture(GL_TEXTURE_2D, m_ntexture);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 
                        0, 0, m_hitn.width(), m_hitn.height(),
                        GL_RGBA, GL_FLOAT, m_hitn.buffer());
                    
                    glActiveTexture(GL_TEXTURE0 +2);
                    glBindTexture(GL_TEXTURE_2D, m_vtexture);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 
                        0, 0, m_hitv.width(), m_hitv.height(),
                        GL_RGBA, GL_FLOAT, m_hitv.buffer());
                }
            }
        }
        
        if(mode == 1)
        {
            glBindVertexArray(m_vao);
            glUseProgram(m_program);
            
            program_uniform(m_program, "ptexture", 0);
            program_uniform(m_program, "ntexture", 1);
            program_uniform(m_program, "vtexture", 2);
            program_uniform(m_program, "pixel", texcoord);
            program_uniform(m_program, "mode", mode);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_ptexture);
            glActiveTexture(GL_TEXTURE0 +1);
            glBindTexture(GL_TEXTURE_2D, m_ntexture);
            glActiveTexture(GL_TEXTURE0 +2);
            glBindTexture(GL_TEXTURE_2D, m_vtexture);
            
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        
        else if(mode == 0)
            draw(m_mesh, m_camera);
        
        //
        if(key_state('s'))
        {
            clear_key_state('s');
            
            static int n= 1;
            char tmp[1024];
            sprintf(tmp, "screenshot%02d.png", n++);
            printf("writing %s...\n", tmp);
            screenshot(tmp);
        }        
        
        return 1;
    }
    
    int quit( )
    {
        m_mesh.release();
        return 0;
    }

    
    // recuperer les sources de lumiere du mesh : triangles associee a une matiere qui emet de la lumiere, material.emission != 0
    int build_sources( )
    {
        for(int i= 0; i < m_mesh.triangle_count(); i++)
        {
            // recupere la matiere associee a chaque triangle de l'objet
            Material material= m_mesh.triangle_material(i);

            if((material.emission.r + material.emission.g + material.emission.b) > 0)
                // inserer la source de lumiere dans l'ensemble.
                m_sources.push_back( Source(m_mesh.triangle(i), material.emission) );
        }

        printf("%d sources.\n", (int) m_sources.size());
        return (int) m_sources.size();
    }

    bool direct( const Ray& ray )
    {
        for(size_t i= 0; i < m_sources.size(); i++)
        {
            float t, u, v;
            if(m_sources[i].intersect(ray, ray.tmax, t, u, v))
                return true;
        }
        
        return false;
    }


    // recuperer les triangles du mesh
    int build_triangles( )
    {
        for(int i= 0; i < m_mesh.triangle_count(); i++)
            m_triangles.push_back( Triangle(m_mesh.triangle(i)) );
        
        printf("%d triangles.\n", (int) m_triangles.size());
        return (int) m_triangles.size();
    }


    // calcule l'intersection d'un rayon et de tous les triangles
    bool intersect( const Ray& ray, Hit& hit )
    {
        hit.t= ray.tmax;
        for(size_t i= 0; i < m_triangles.size(); i++)
        {
            float t, u, v;
            if(m_triangles[i].intersect(ray, hit.t, t, u, v))
            {
                hit.t= t;
                hit.u= u;
                hit.v= v;
                
                hit.p= ray(t);      // evalue la positon du point d'intersection sur le rayon
                hit.n= m_triangles[i].normal(u, v);
                
                hit.object_id= i;	// permet de retrouver toutes les infos associees au triangle
            }
        }
        
        return (hit.object_id != -1);
    }

protected:
    Mesh m_mesh;
    Orbiter m_camera;

    std::vector<Triangle> m_triangles;
    std::vector<Source> m_sources;

    Image m_hitp;
    Image m_hitn;
    Image m_hitv;

    GLuint m_vao;
    GLuint m_program;
    
    GLuint m_ptexture;
    GLuint m_ntexture;
    GLuint m_vtexture;
};


int main( int argc, char **argv )
{
    const char *filename= "cornell.obj";
    if(argc > 1)
        filename= argv[1];
    
    IS app(filename);
    app.run();
    
    return 0;
}
