
#include "draw.h"
#include "window.h"


void draw( Mesh& m, const Orbiter& camera )
{
    // recupere les transformations
    Transform model= make_identity();
    Transform view= orbiter_view_transform(camera);
    Transform projection= orbiter_projection_transform(camera, window_width(), window_height(), 45);
    
    // affiche l'objet
    draw(m, model, view, projection);
}

void draw( Mesh& m, const Orbiter& camera, GLuint texture )
{
    // recupere les transformations
    Transform model= make_identity();
    Transform view= orbiter_view_transform(camera);
    Transform projection= orbiter_projection_transform(camera, window_width(), window_height(), 45);
    
    // affiche l'objet
    draw(m, model, view, projection, texture);
}

