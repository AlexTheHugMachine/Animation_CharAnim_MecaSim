
#ifndef _OBJ_H
#define _OBJ_H

#include "mesh.h"

//! charge un fichier wavefront .obj et renvoie un mesh compose de triangles non indexes. utiliser glDrawArrays pour l'afficher.
mesh read_mesh( const char *filename );

#endif
