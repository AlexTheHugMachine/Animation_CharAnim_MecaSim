
#ifndef _DRAW_H
#define _DRAW_H

#include "mesh.h"
#include "orbiter.h"
#include "window.h"
#include "program.h"


//! \addtogroup objet3D
///@{

//! \file

//! dessine l'objet avec les transformations model, vue et projection.
void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection );
//! applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet.
void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection, const GLuint texture );

//! dessine l'objet avec les transformations vue et projection, definies par la camera. model est la transformation identite.
void draw( Mesh& m, Orbiter& camera );
//! dessine l'objet avec une transformation model. les transformations vue et projection sont celles de la camera
void draw( Mesh& m, const Transform& model, Orbiter& camera );

//! dessine l'objet avec les transformations vue et projection. model est l'identite. applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet.
void draw( Mesh& m, Orbiter& camera, const GLuint texture );
//! dessine l'objet avec une transformation model. les transformations vue et projection sont celles de la camera. applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet.
void draw( Mesh& m, const Transform& model, Orbiter& camera, const GLuint texture );

/*! representation des options / parametres d'un draw.
    permet de donner tous les parametres d'un draw de maniere flexible.

    exemple :
    \code
    Mesh objet= { ... };

    DrawParam param;
    param.light(Point(0, 20, 0), Red());
    param.camera(orbiter);
    param.draw(objet);
    \endcode

    ou de maniere encore plus compacte :
    \code
    DrawParam().light(Point(0, 20, 0), Red()).model(m).camera(orbiter).draw(objet);
    \endcode
    les parametres peuvent etre decrits dans un ordre quelconque, mais DrawParam::draw() doit etre appele en dernier.
 */
class DrawParam
{
public:
    //! constructeur par defaut.
    DrawParam( ) : m_model(), m_view(), m_projection(),
        m_use_light(false), m_light(), m_light_color(),
        m_use_texture(false), m_texture(0),
        m_use_alpha_test(false), m_alpha_min(0.3f)
    {}

    //! modifie la transformation model utilisee pour afficher l'objet.
    DrawParam& model( const Transform& m ) { m_model= m; return *this; }
    //! modifie la transformation view utilisee pour afficher l'objet.
    DrawParam& view( const Transform& m ) { m_view= m; return *this; }
    //! modifie la transformation projection utilisee pour afficher l'objet.
    DrawParam& projection( const Transform& m ) { m_projection= m; return *this; }

    //! utilise les transformations view et projection definies par une camera.
    DrawParam& camera( Orbiter& o ) { m_view= o.view(); m_projection= o.projection(window_width(), window_height(), 45); return *this; }
    //! utilise les transformations view et projection definies par une camera. parametres explicites de la projection.
    DrawParam& camera( Orbiter& o, const int width, const int height, const float fov ) { m_view= o.view(); m_projection= o.projection(width, height, fov); return *this; }
    //! eclaire l'objet avec une source ponctuelle, de position p et de couleur c.
    DrawParam& light( const Point& p, const Color& c= White() ) { m_use_light= true; m_light= p; m_light_color=c; return *this; }
    //! plaque une texture a la surface de l'objet.
    DrawParam& texture( const GLuint t ) { m_use_texture= true; m_texture= t; return *this; }

    //! texture semi transparente, si l'alpha du texel est plus petit que alpha_min, le pixel est transparent. desactive aussi les calculs d'eclairage.
    DrawParam& alpha( const float a=0.f ) { m_use_alpha_test= (a>0.f); m_alpha_min= a; return *this; }

    //! Use light: on/off
    DrawParam& lighting(bool use_light=true) { m_use_light=use_light;  return *this; }

    //! dessine l'objet avec l'ensemble des parametres definis.
    void draw( Mesh& mesh );
    
    //! renvoie la position de la lumi�re
    const Point& light() const { return m_light; }
    
protected:
    /*! construit un shader program configure.
    \param use_texcoord force l'utilisation des coordonnees de texture
    \param use_normal force l'utilisation des normales
    \param use_color force l'utilisation des couleurs 
    \param use_light force l'utilisation d'un source de lumiere 
    \param use_alpha_test force l'utilisation d'un test de transparence, cf utilisation d'une texture avec un canal alpha
     */
    GLuint create_program( const GLenum primitives, const bool use_texcoord, const bool use_normal, const bool use_color, const bool use_light, const bool use_alpha_test );
    
    Transform m_model;
    Transform m_view;
    Transform m_projection;

    bool m_use_light;
    Point m_light;
    Color m_light_color;

    bool m_use_texture;
    GLuint m_texture;

    bool m_use_alpha_test;
    float m_alpha_min;
};


//! description d'un shader program compile.
struct PipelineProgram
{
    std::string filename;
    std::string definitions;
    GLuint program;
};

//! ensemble de shader programs compiles. singleton.
class PipelineCache
{
public:
    ~PipelineCache( ) 
    {
        printf("[pipeline cache] %d programs.\n", int(m_programs.size()));
        // le contexte est deja detruit lorsque ce destructeur est appelle... 
        // trop tard pour detruire les programs et les shaders...
    }
    
    //! renvoie un shader program compile.
    PipelineProgram *find( const char *filename, const char *definitions= "" )
    {
        for(auto program : m_programs)
        {
            if(program->filename == filename && program->definitions == definitions)
                return program;
        }
        
        // cree le programme s'il n'existe pas deja...
        GLuint p= read_program(filename, definitions);
        program_print_errors(p);
        
        PipelineProgram *program= new PipelineProgram {filename, definitions, p};
        m_programs.push_back(program);
        return program;
    }
    
    //! access au singleton.
    static PipelineCache& manager( ) 
    {
        static PipelineCache cache;
        return cache;
    }
    
protected:
    //! destructeur prive. le singleton ne sera detruit qu'a la fin de l'application... fonctionne correctement avec la classe App. doit etre detruit tant que le contexte openGL existe.
    PipelineCache( ) : m_programs() {}
    
    std::vector<PipelineProgram *> m_programs;
};


///@}
#endif
