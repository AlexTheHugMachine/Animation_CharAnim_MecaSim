
#ifndef _DRAW_H
#define _DRAW_H

#include "mesh.h"
#include "orbiter.h"


//! \addtogroup objet3D
///@{

//! \file 
//! dessine un objet du point de vue d'une camera

//! dessine l'objet avec les transformations vue et projection, definies par la camera. model est la transformation identite.
void draw( Mesh& m, const Orbiter& camera );
//! dessine l'objet avec une transformation model. les transformations vue et projection sont celles de la camera
void draw( Mesh& m, const Transform& model, const Orbiter& camera );

//! dessine l'objet avec les transformations vue et projection. model est l'identite. applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet. 
void draw( Mesh& m, const Orbiter& camera, GLuint texture );
//! dessine l'objet avec une transformation model. les transformations vue et projection sont celles de la camera. applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet. 
void draw( Mesh& m, const Transform& model, const Orbiter& camera, GLuint texture );

///@}
#endif
