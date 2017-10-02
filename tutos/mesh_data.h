
//! \file mesh_data.h representation des donnees d'un fichier wavefront .obj


#ifndef _MESH_DATA_H
#define _MESH_DATA_H

#include "mesh.h"


struct MeshData
{
    std::vector<vec3> positions;
    std::vector<vec2> texcoords;
    std::vector<vec3> normals;
    
    std::vector<int> position_indices;
    std::vector<int> texcoord_indices;
    std::vector<int> normal_indices;
    
    std::vector<Material> materials;
    std::vector<int> material_indices;
};


//! charge un fichier wavefront .obj et renvoie les donnees.
MeshData read_mesh_data( const char *filename );

//! renvoie l'englobant.
void bounds( const MeshData& data, Point& pmin, Point& pmax );

//! (re-) calcule les normales des sommets. utiliser avant les reindexations, cf indices() et vertices().
void normals( MeshData& data );

//! construit les sommets. prepare l'affichage openGL, avec glDrawArrays().
MeshData vertices( const MeshData& data );


#endif
