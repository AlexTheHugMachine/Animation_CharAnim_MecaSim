
#include "draw.h"
#include "window.h"


void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection, const GLuint texture )
{
    m.draw(model, view, projection, /* use light */ false, Point(), White(), /* use texture */ true, texture);
}

void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection )
{
    m.draw(model, view, projection, /* use light */ false, Point(), White(), /* use texture */ false, 0);
}

void draw( Mesh& m, const Transform& model, const Orbiter& camera, const GLuint texture )
{
    // recupere les transformations
    Transform view= camera.view();
    Transform projection= camera.projection((float) window_width(), (float) window_height(), 45);
    
    // affiche l'objet
    draw(m, model, view, projection, texture);
}

void draw( Mesh& m, const Orbiter& camera, const GLuint texture )
{
    draw(m, Identity(), camera, texture);
}

void draw( Mesh& m, const Orbiter& camera )
{
    draw(m, Identity(), camera, 0);
}

void draw( Mesh& m, const Transform& model, const Orbiter& camera )
{
    draw(m, model, camera, 0);
}

void DrawParam::draw( Mesh& mesh ) const
{
    mesh.draw(m_model, m_view, m_projection, /* use light */ m_use_light, m_light, m_light_color, /* use texture */ m_use_texture, m_texture);
}

void draw( Mesh& m, const DrawParam& param )
{
    param.draw(m);
}
