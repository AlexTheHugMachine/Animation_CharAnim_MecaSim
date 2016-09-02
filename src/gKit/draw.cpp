
#include "draw.h"
#include "window.h"


void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection, const GLuint texture )
{
    m.draw(model, view, projection, texture);
}

void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection )
{
    m.draw(model, view, projection, 0);
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
