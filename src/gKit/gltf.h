
#ifndef _GLTF_MESH_H
#define _GLTF_MESH_H

#include <vector>

#include "vec.h"
#include "color.h"
#include "mesh.h"
#include "image_io.h"


//! charge un fichier .gltf et construit un mesh statique, sans animation.
Mesh read_gltf_mesh( const char *filename );

//! description d'une camera.
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

//! description d'une source de lumiere.
struct GLTFLight
{
    Point position;
    Color emission;
    float intensity;
    
    // \todo transform et les autres parametres...
};

//! charge un fichier .gltf et renvoie les sources de lumiere ponctuelles.
std::vector<GLTFLight> read_gltf_lights( const char *filename );

//! description d'une matiere pbr.
struct GLTFMaterial
{
    Color color;
    Color emission;
    float metallic;
    float roughness;
    float transmission;
    float ior;
    // \todo volume...;
    
    int color_texture;              //<! indice de la texture ou -1.
    int metallic_roughness_texture; //<! indice de la texture ou -1.
    int emission_texture;           //<! indice de la texture ou -1.
    int occlusion_texture;          //<! indice de la texture ou -1.    // \todo probablement pas necessaire, cf canal rouge de metallic_roughness_texture
    int normal_texture;             //<! indice de la texture ou -1.
};

//! charge un fichier .gltf et renvoie les matieres.
std::vector<GLTFMaterial> read_gltf_materials( const char *filename );

//! charge un fichier .gltf et charge les images referencees par les matieres.
std::vector<ImageData> read_gltf_images( const char *filename );


//! groupe de triangles d'un maillage. chaque groupe est associe a une matiere.
struct GLTFPrimitives
{
    int primitives_mode;    //!< triangles.
    int primitives_index;   //<! indice unique.
    int material_index;     //<! indice de la matiere des primitives.
    
    // buffers...
    std::vector<unsigned> indices;
    std::vector<vec3> positions;
    std::vector<vec2> texcoords;
    std::vector<vec3> normals;
};

//! description d'un maillage.
struct GLTFMesh
{
    std::vector<GLTFPrimitives> primitives; //!< groupes de triangles associes a une matiere.
};

//! instances d'un maillage.
struct GLTFInstances
{
    std::vector<Transform> transforms;      //!< transformations model.
    int mesh_index;                         //!< indice du maillage.
};

//! position et orientation d'un maillage dans la scene.
struct GLTFNode
{
    Transform model;                        //!< transformation model.
    int mesh_index;                         //!< indice du maillage.
};


/*! representation d'une scene statique glTF.
    resume : https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/README.md

    specification : https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html

    parser : https://github.com/jkuhlmann/cgltf

    une scene est un ensemble de maillages places et orientes dans un repere.
    un GLTFNode permet de dessiner un maillage GLTFMesh a sa place.
    un maillage est un ensemble de groupes de triangles / primitives. cf GLTFPrimitives.
    un groupe de primitives est associe a une matiere. cf GLTFMaterial.
 */
struct GLTFScene
{
    std::vector<GLTFMesh> meshes;           //!< ensemble de maillages.
    std::vector<GLTFNode> nodes;            //!< noeuds / position et orientation des maillages.
    
    std::vector<GLTFMaterial> materials;    //!< matieres.
    std::vector<GLTFLight> lights;          //!< lumieres.
    std::vector<GLTFCamera> cameras;        //!< cameras.
    
    void bounds( Point& pmin, Point& pmax) const;   //!< calcule les points extremes de la scene, utile pour regler un orbiter.
    std::vector<GLTFInstances> instances( ) const;  //!< regroupe les instances de chaque maillage.
};

//! charge un fichier .gltf et construit une scene statique, sans animation.
GLTFScene read_gltf_scene( const char *filename );

#endif

