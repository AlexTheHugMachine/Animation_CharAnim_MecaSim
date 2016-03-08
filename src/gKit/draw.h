
#ifndef _DRAW_H
#define _DRAW_H

#include "mesh.h"
#include "orbiter.h"


//! \addtogroup objet3D
///@{

//! \file 
//! dessine un objet du point de vue d'une camera

//! dessine l'objet avec les transformations model, vue et projection, definies par la camera
void draw( Mesh& m, const Orbiter& camera );
//! dessine l'objet avec les transformations model, vue et projection. applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet. 
void draw( Mesh& m, const Orbiter& camera, GLuint texture );

///@}
#endif
