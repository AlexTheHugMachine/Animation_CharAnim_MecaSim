
#include "draw.h"
#include "window.h"


void draw( Mesh& m, const Transform& model, const Orbiter& camera, GLuint texture )
{
    // recupere les transformations
    Transform view= orbiter_view_transform(camera);
    Transform projection= orbiter_projection_transform(camera, window_width(), window_height(), 45);
    
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
