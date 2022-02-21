
#ifndef _GLTF_MESH_H
#define _GLTF_MESH_H

#include "mesh.h"

//! charge un fichier .gltf et construit un mesh statique, sans animation.
Mesh read_gltf_mesh( const char *filename );

#if 0
//a voir, pas sur que ce soit le plus pratique...
struct StaticScene
{
    Mesh mesh;
    
    Transform view;
    Transform projection;
    
    //~ // camera
    //~ Point position;
    //~ float znear;
    //~ float zfar;
    //~ float aspect;
    //~ float fov;
    
    struct Light
    {
        Point position;
        Color emission;
    };

    std::vector<Light> lights;
    
    struct Material
    {
        Color color;
        Color emission;
        float metallic;
        float roughness;
        
        int color_texture;
        int metallic_roughness_texture;
        int emission_texture;
        int occlusion_texture;
        int normal_texture;
    };
    
    std::vector<Material> materials;
    
    //~ std::vector<Image> images;
    //~ std::vector<GLuint> textures;
};

//! charge un fichier .gltf, sans animation.
StaticScene read_static_scene( const char *filename );

//~ // autre solution ? 
//~ // un peu bourrin, mais... 
//~ read_gltf_camera( const char *filename );       
//~ read_gltf_lights( const char *filename );
//~ read_gltf_materials( const char *filename );
#endif

#endif

