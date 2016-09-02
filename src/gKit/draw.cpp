
#include "draw.h"
#include "window.h"


void draw( Mesh& m, const Transform& model, const Orbiter& camera, GLuint texture )
{
    // recupere les transformations
    Transform view= camera.view();
    Transform projection= camera.projection((float) window_width(), (float) window_height(), 45);
    
    // affiche l'objet
    draw(m, model, view, projection, texture);
}

void draw( Mesh& m, const Orbiter& camera, GLuint texture )
{
    draw(m, make_identity(), camera, texture);
}

void draw( Mesh& m, const Orbiter& camera )
{
    draw(m, make_identity(), camera, 0);
}

void draw( Mesh& m, const Transform& model, const Orbiter& camera )
{
    draw(m, model, camera, 0);
}
