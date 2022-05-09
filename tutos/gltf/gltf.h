
#ifndef _GLTF_MESH_H
#define _GLTF_MESH_H

#include <vector>

#include "vec.h"
#include "color.h"
#include "mesh.h"
#include "image_io.h"


//! charge un fichier .gltf et construit un mesh statique, sans animation.
Mesh read_gltf_mesh( const char *filename );


struct GLTFCamera
{
    float fov;
    float aspect;
    float znear;
    float zfar;
    
    Transform view;
    Transform projection;
};

//! charge un fichier .gltf renvoie les cameras.
std::vector<GLTFCamera> read_gltf_cameras( const char *filename );


struct GLTFLight
{
    Point position;
    Color emission;
    float intensity;
};

//! charge un fichier .gltf et renvoie les sources de lumiere ponctuelles.
std::vector<GLTFLight> read_gltf_lights( const char *filename );


struct GLTFMaterial
{
    Color color;
    Color emission;
    float metallic;
    float roughness;

    int color_texture;              //<! indice de la texture ou -1.
    int metallic_roughness_texture; //<! indice de la texture ou -1.
    int emission_texture;           //<! indice de la texture ou -1.
    int occlusion_texture;          //<! indice de la texture ou -1.    // probablement pas necessaire, cf canal rouge de metallic_roughness_texture
    int normal_texture;             //<! indice de la texture ou -1.
};

//! charge un fichier .gltf et renvoie les matieres.
std::vector<GLTFMaterial> read_gltf_materials( const char *filename );

//! charge un fichier .gltf et charge les images referencees par les matieres.
std::vector<ImageData> read_gltf_images( const char *filename );

#endif

