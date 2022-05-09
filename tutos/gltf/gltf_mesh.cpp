

//~ #define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include "gltf_mesh.h"


Mesh read_gltf_mesh( const char *filename )
{
    printf("loading glTF mesh '%s'...\n", filename);
    
    cgltf_options options= { };
    cgltf_data *data= nullptr;
    cgltf_result code= cgltf_parse_file(&options, filename, &data);
    if(code != cgltf_result_success)
    {
        printf("[error] loading glTF mesh '%s'...\n", filename);
        return Mesh::error();
    }
    
    if(cgltf_validate(data) != cgltf_result_success)
    {
        printf("[error] invalid glTF mesh '%s'...\n", filename);
        return Mesh::error();
    }
    //~ printf("validating glTF mesh... done\n");
    
    printf("meshs %ld\n", data->meshes_count);
    //~ printf("materials %ld\n", data->materials_count);
    //~ printf("accessors %ld\n", data->accessors_count);
    //~ printf("buffer views %ld\n", data->buffer_views_count);
    //~ printf("buffers %ld\n", data->buffers_count);
    //~ printf("cameras %ld\n", data->cameras_count);
    printf("lights %ld\n", data->lights_count);
    printf("nodes %ld\n", data->nodes_count);
    printf("scenes %ld\n", data->scenes_count);
    //~ printf("animations %ld\n", data->animations_count);
    //~ printf("skins %ld\n", data->skins_count);
    
    // load buffers
    //~ printf("loading buffers...\n");
    code= cgltf_load_buffers(&options, data, filename);
    if(code != cgltf_result_success)
    {
        printf("[error] loading glTF buffers...\n");
        return Mesh::error();
    }
    //~ printf("loading buffers... done\n");
    
    // 
    std::vector<unsigned> indices;
    std::vector<int> material_indices;
    std::vector<vec3> positions;
    std::vector<vec2> texcoords;
    std::vector<vec3> normals;
    
    Materials materials;
    
    // load images
#if 1
    printf("loading images %ld...\n", data->images_count);
    for(unsigned i= 0; i < data->images_count; i++)
    {
        printf("[%u] %s\n", i, data->images[i].uri);
        
        if(data->images[i].uri)
            materials.insert_texture(data->images[i].uri);
        
        else
        {
            char tmp[1024];
            sprintf(tmp, "image%u", i);
            materials.insert_texture(tmp);
        }
    }
#endif
    
    // materials
    printf("materials %ld...\n", data->materials_count);
    for(unsigned i= 0; i < data->materials_count; i++)
    {
        cgltf_material *material= &data->materials[i];
        
        printf("[%u] %s\n", i, material->name);
        materials.insert( Material(Color(0.8)), material->name );
        
    #if 0
        gltf::Material m;
        m.color= Color(0.8, 0.8, 0.8, 1);
        m.color_texture= -1;
        m.metallic_roughness_texture= -1;
        m.occlusion_texture= -1;
        m.normal_texture= -1;
        
        printf("%u: '%s'\n", i, material->name);
        if(material->has_pbr_metallic_roughness)
        {
            printf("  pbr metallic roughness\n");
            cgltf_pbr_metallic_roughness *pbr= &material->pbr_metallic_roughness;
            printf("    base color %f %f %f %f\n", pbr->base_color_factor[0], pbr->base_color_factor[1], pbr->base_color_factor[2], pbr->base_color_factor[3]);
            printf("      texture %d\n", pbr->base_color_texture.texture ? int(std::distance(data->images, pbr->base_color_texture.texture->image)) : -1);
            printf("    metallic %f, roughness %f\n", pbr->metallic_factor, pbr->roughness_factor);
            printf("      texture %d\n", pbr->metallic_roughness_texture.texture ? int(std::distance(data->images, pbr->metallic_roughness_texture.texture->image)) : -1);
            
            m.color= Color(pbr->base_color_factor[0], pbr->base_color_factor[1], pbr->base_color_factor[2], pbr->base_color_factor[3]);
            if(pbr->base_color_texture.texture && pbr->base_color_texture.texture->image)
                m.color_texture= int(std::distance(data->images, pbr->base_color_texture.texture->image));
            
            m.metallic= pbr->metallic_factor;
            m.roughness= pbr->roughness_factor;
            if(pbr->metallic_roughness_texture.texture && pbr->metallic_roughness_texture.texture->image)
                m.metallic_roughness_texture= int(std::distance(data->images, pbr->metallic_roughness_texture.texture->image));
        }
        if(material->has_clearcoat)
            printf("  clearcoat\n");
        if(material->has_sheen)
            printf("  sheen\n");
        if(material->normal_texture.texture && material->normal_texture.texture->image)
        {
            printf("  normal texture %d\n", int(std::distance(data->images, material->normal_texture.texture->image)));
            m.normal_texture= int(std::distance(data->images, material->normal_texture.texture->image));
        }
        if(material->occlusion_texture.texture && material->occlusion_texture.texture->image)
        {
            printf("  occlusion texture %d\n", int(std::distance(data->images, material->occlusion_texture.texture->image)));
            m.occlusion_texture= int(std::distance(data->images, material->occlusion_texture.texture->image));
        }
        if(material->emissive_texture.texture && material->emissive_texture.texture->image)
            printf("  emissive texture %d\n", int(std::distance(data->images, material->emissive_texture.texture->image)));
        
        if(material->has_pbr_specular_glossiness)
            printf("  pbr  specular glossiness\n");
            
        m_materials.push_back(m);
    #endif
    }
    
    if(data->cameras_count > 0)
    {
        printf("cameras %ld...\n", data->cameras_count);
        
        // recupere la premiere camera...
        cgltf_camera *camera= &data->cameras[0];
        printf("[%u] '%s'\n", 0, camera->name);
        if(camera->type == cgltf_camera_type_perspective)
        {
            cgltf_camera_perspective *perspective= &camera->data.perspective;
            if(perspective->has_aspect_ratio)
                printf("  aspect ratio %f\n", perspective->aspect_ratio);
            printf("  yfov %f\n", perspective->yfov);
            printf("  znear %f", perspective->znear);
            if(perspective->has_zfar)
                printf(", zfar %f", perspective->zfar);
            printf("\n");
        }
    }
    
    // retrouve la transformation associee a la camera 0
    for(unsigned i= 0; i < data->nodes_count; i++)
    {
        cgltf_node *node= &data->nodes[i];
        if(node->camera != nullptr)
        {
            if(node->camera == data->cameras)
            {
                float model_matrix[16];
                cgltf_node_transform_world(node, model_matrix); // transformation globale
                
                Transform model;
                model.column_major(model_matrix);       // gltf organise les 16 floats par colonne...
                
                // view= inverse(model)
                Transform view= Inverse(model);
                printf("camera[0] attached to node[%u]...\n", i);
            }
        }
    }
    
    printf("lights %ld...\n", data->lights_count);
    for(unsigned i= 0; i < data->lights_count; i++)
    {
        cgltf_light *light= &data->lights[i];
        
        printf("[%u] '%s'\n", i, light->name);
        printf("  emission %f %f %f\n", light->color[0], light->color[1], light->color[2]);
        printf("  intensity %f\n", light->intensity);
    }
    
    // retrouve la transformation associee aux sources
    for(unsigned i= 0; i < data->nodes_count; i++)
    {
        cgltf_node *node= &data->nodes[i];
        if(node->light != nullptr)
        {
            float model_matrix[16];
            cgltf_node_transform_world(node, model_matrix); // transformation globale
            
            Transform model;
            model.column_major(model_matrix);       // gltf organise les 16 floats par colonne...
            
            int light_id= int(std::distance(data->lights, node->light));
            printf("light[%u] attached to node[%u]...\n", light_id, i);
        }
    }
    
    
    // parcourir les noeuds de la scene et transformer les meshs associes aux noeuds...
    std::vector<float> buffer;
    for(unsigned node_id= 0; node_id < data->nodes_count; node_id++)
    {
        cgltf_node *node= &data->nodes[node_id];
        if(node->mesh== nullptr)
            // pas de mesh associe
            continue;
        
        // transformation vers la scene
        float model_matrix[16];
        cgltf_node_transform_world(node, model_matrix); // transformation globale
        
        Transform model;
        model.column_major(model_matrix);       // gltf organise les 16 floats par colonne...
        Transform normal= model.normal();       // transformation pour les normales
        
        cgltf_mesh *mesh= node->mesh;
        for(unsigned primitive_id= 0; primitive_id < mesh->primitives_count; primitive_id++)
        {
            cgltf_primitive *primitives= &mesh->primitives[primitive_id];
            assert(primitives->type == cgltf_primitive_type_triangles);
            
            // matiere associee au groupe de triangles
            int material_id= -1;
            if(primitives->material)
            {
                material_id= int(std::distance(data->materials, primitives->material));
                assert(material_id < materials.count());
                assert(materials.find(primitives->material->name) != -1);
            }
            
            // indices
            if(primitives->indices)
            {
                unsigned offset= positions.size();
                for(unsigned i= 0; i < primitives->indices->count; i++)
                    indices.push_back(offset + cgltf_accessor_read_index(primitives->indices, i));
                
                // un indice de matiere par triplet d'indices / par triangle
                for(unsigned i= 0; i+2 < primitives->indices->count; i+= 3)
                    material_indices.push_back(material_id);
            }
            
            // attributs
            for(unsigned attribute_id= 0; attribute_id < primitives->attributes_count; attribute_id++)
            {
                cgltf_attribute *attribute= &primitives->attributes[attribute_id];
                if(attribute->type == cgltf_attribute_type_position)
                {
                    assert(attribute->data->type == cgltf_type_vec3);
                    
                    buffer.resize(cgltf_accessor_unpack_floats(attribute->data, nullptr, 0));
                    cgltf_accessor_unpack_floats(attribute->data, buffer.data(), buffer.size());
                    
                    // transforme les positions des sommets
                    for(unsigned i= 0; i+2 < buffer.size(); i+= 3)
                        positions.push_back( model(Point(buffer[i], buffer[i+1], buffer[i+2])) );
                }
                if(attribute->type == cgltf_attribute_type_normal)
                {
                    assert(attribute->data->type == cgltf_type_vec3);
                    
                    buffer.resize(cgltf_accessor_unpack_floats(attribute->data, nullptr, 0));
                    cgltf_accessor_unpack_floats(attribute->data, buffer.data(), buffer.size());
                    
                    // transforme les normales des sommets
                    for(unsigned i= 0; i+2 < buffer.size(); i+= 3)
                        normals.push_back( normal(Vector(buffer[i], buffer[i+1], buffer[i+2])) );
                }
                if(attribute->type == cgltf_attribute_type_texcoord)
                {
                    assert(attribute->data->type == cgltf_type_vec2);
                    
                    buffer.resize(cgltf_accessor_unpack_floats(attribute->data, nullptr, 0));
                    cgltf_accessor_unpack_floats(attribute->data, buffer.data(), buffer.size());
                    
                    for(unsigned i= 0; i+1 < buffer.size(); i+= 2)
                        texcoords.push_back( vec2(buffer[i], buffer[i+1]) );
                }
            }
        }
    }
    
    cgltf_free(data);
    
    // reconstruit le mesh...
    Mesh mesh(GL_TRIANGLES);
    
    // 1. les sommets et les attributs, si necessaire...
    bool has_texcoords= (texcoords.size() == positions.size());
    bool has_normals= (normals.size() == positions.size());
    for(unsigned i= 0; i < positions.size(); i++)
    {
        if(has_texcoords) mesh.texcoord(texcoords[i]);
        if(has_normals) mesh.normal(normals[i]);
        mesh.vertex(positions[i]);
    }
    
    // 2. les triangles et leurs matieres, si necessaire...
    mesh.materials(materials);
    bool has_materials= (materials.count() > 0);
    for(unsigned i= 0; i +2 < indices.size(); i+=3)
    {
        if(has_materials) mesh.material(material_indices[i / 3]);
        
        mesh.index(indices[i]);
        mesh.index(indices[i+1]);
        mesh.index(indices[i+2]);
    }
    
    return mesh;
}

