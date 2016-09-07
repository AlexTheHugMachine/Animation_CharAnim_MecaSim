
#ifndef _DRAW_H
#define _DRAW_H

#include "mesh.h"
#include "orbiter.h"
#include "window.h"


//! \addtogroup objet3D
///@{

//! \file 
//! dessine un objet du point de vue d'une camera

//! dessine l'objet avec les transformations model, vue et projection.
void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection );
//! applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet. 
void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection, const GLuint texture );

//! dessine l'objet avec les transformations vue et projection, definies par la camera. model est la transformation identite.
void draw( Mesh& m, const Orbiter& camera );
//! dessine l'objet avec une transformation model. les transformations vue et projection sont celles de la camera
void draw( Mesh& m, const Transform& model, const Orbiter& camera );

//! dessine l'objet avec les transformations vue et projection. model est l'identite. applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet. 
void draw( Mesh& m, const Orbiter& camera, const GLuint texture );
//! dessine l'objet avec une transformation model. les transformations vue et projection sont celles de la camera. applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet. 
void draw( Mesh& m, const Transform& model, const Orbiter& camera, const GLuint texture );


class DrawParam
{
public:
    DrawParam( ) : m_model(), m_view(), m_projection(), m_use_light(false), m_light(), m_light_color(), m_use_texture(false), m_texture(0) {}
    DrawParam& model( const Transform& m ) { m_model= m; return *this; }
    DrawParam& camera( const Orbiter& o ) { m_view= o.view(); m_projection= o.projection(window_width(), window_height(), 45); return *this; }
    DrawParam& light( const Point& p, const Color& c= White() ) { m_use_light= true; m_light= p; m_light_color=c; return *this; }
    DrawParam& texture( const GLuint t ) { m_use_texture= true; m_texture= t; return *this; }
    
    void draw( Mesh& mesh ) const;

protected:
    Transform m_model;
    Transform m_view;
    Transform m_projection;
    
    bool m_use_light;
    Point m_light;
    Color m_light_color;
    
    bool m_use_texture;
    GLuint m_texture;
    
    //~ bool m_use_color;
    //~ Color m_mesh_color;
    
};

void draw( Mesh& mesh, const DrawParam& param );

/*!
\code
    DrawParam param;
    draw(mesh, param.camera(camera).light(Point(0, 10, 0), Blue()).texture(t));

// ou
    DrawParam().camera(camera).light(Point(0, 10, 0), Blue()).draw(mesh);
\endcode
*/

///@}
#endif
