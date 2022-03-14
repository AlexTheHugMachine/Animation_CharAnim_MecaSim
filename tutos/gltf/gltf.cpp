
#include "files.h"
#include "texture.h"

#include "cgltf.h"
#include "gltf.h"


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
    
    code= cgltf_load_buffers(&options, data, filename);
    if(code != cgltf_result_success)
    {
        printf("[error] loading glTF buffers...\n");
        return Mesh::error();
    }
    
    // 
    std::vector<unsigned> indices;
    std::vector<int> material_indices;
    std::vector<vec3> positions;
    std::vector<vec2> texcoords;
    std::vector<vec3> normals;
    
    Materials materials;
    
    // textures
    for(unsigned i= 0; i < data->images_count; i++)
        if(data->images[i].uri)
            materials.insert_texture(data->images[i].uri);
    
    // materials
    for(unsigned i= 0; i < data->materials_count; i++)
    {
        cgltf_material *material= &data->materials[i];
        //~ printf("materials[%u]: '%s'\n", i, material->name);
        
        Material m(Color(0.8));
        if(material->has_pbr_metallic_roughness)
        {
            cgltf_pbr_metallic_roughness *pbr= &material->pbr_metallic_roughness;
            //~ printf("  pbr metallic roughness\n");
            //~ printf("    base color %f %f %f %f\n", pbr->base_color_factor[0], pbr->base_color_factor[1], pbr->base_color_factor[2], pbr->base_color_factor[3]);
            //~ printf("      texture %d\n", pbr->base_color_texture.texture ? int(std::distance(data->images, pbr->base_color_texture.texture->image)) : -1);
            //~ printf("    metallic %f, roughness %f\n", pbr->metallic_factor, pbr->roughness_factor);
            //~ printf("      texture %d\n", pbr->metallic_roughness_texture.texture ? int(std::distance(data->images, pbr->metallic_roughness_texture.texture->image)) : -1);
            
            Color color= Color(pbr->base_color_factor[0], pbr->base_color_factor[1], pbr->base_color_factor[2], pbr->base_color_factor[3]);
            float metallic= pbr->metallic_factor;
            float roughness= pbr->roughness_factor;
            
            // conversion metallic-roughness vers diffuse-specular + Blinn-Phong
            // metaux { diffuse= black, specular= color }
            // non - metaux { diffuse= color, specular= 0.04 }
            m.diffuse= color * (1 - metallic) + metallic * Black();
            m.specular= Color(0.04) * (1 - metallic) + color * metallic;
            
            // conversion roughness vers exposant Blinn-Phong
            m.ns= 2 / (roughness * roughness) - 2;
            if(m.ns == 0)
                m= Material(m.specular);
            // les valeurs sont habituellement dans les textures metallic_roughness... utiliser une matiere diffuse + texture...
            
            //~ printf("  diffuse %f %f %f, specular %f %f %f, ns %f\n", 
                //~ m.diffuse.r, m.diffuse.g, m.diffuse.b,
                //~ m.specular.r, m.specular.g, m.specular.b,
                //~ m.ns);
            
            if(pbr->base_color_texture.texture && pbr->base_color_texture.texture->image)
                m.diffuse_texture= materials.find_texture(pbr->base_color_texture.texture->image->uri);
        }
        
        materials.insert(m, material->name);
    }
    
    
    std::vector<float> buffer;
    // parcourir les noeuds de la scene et transformer les meshs associes aux noeuds...
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
        // parcourir les groupes de triangles du mesh...
        for(unsigned primitive_id= 0; primitive_id < mesh->primitives_count; primitive_id++)
        {
            cgltf_primitive *primitives= &mesh->primitives[primitive_id];
            assert(primitives->type == cgltf_primitive_type_triangles);
            
            // matiere associee au groupe de triangles
            int material_id= -1;
            if(primitives->material)
            {
                assert(material_id < materials.count());
                assert(materials.find(primitives->material->name) != -1);
                material_id= materials.find(primitives->material->name);
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
        
        mesh.triangles(indices[i], indices[i+1], indices[i+2]);
    }
    // \bug si les triangles ne sont pas indexes, pas de matieres dans le mesh...
    
    return mesh;
}


std::vector<GLTFCamera> read_gltf_camera( const char *filename )
{
    printf("loading glTF camera '%s'...\n", filename);
    
    cgltf_options options= { };
    cgltf_data *data= nullptr;
    cgltf_result code= cgltf_parse_file(&options, filename, &data);
    if(code != cgltf_result_success)
    {
        printf("[error] loading glTF mesh '%s'...\n", filename);
        return {};
    }
    
    if(cgltf_validate(data) != cgltf_result_success)
    {
        printf("[error] invalid glTF mesh '%s'...\n", filename);
        return {};
    }
    
    if(data->cameras_count == 0)
    {
        printf("[warning] no camera...\n");
        return {};
    }
    
    cgltf_camera *camera= &data->cameras[0];
    if(camera->type != cgltf_camera_type_perspective)
    {
        printf("[warning] no perspective camera...\n");
        return {};
    }
    
    // retrouve la transformation associee a la camera 0
    for(unsigned i= 0; i < data->nodes_count; i++)
    {
        cgltf_node *node= &data->nodes[i];
        if(node->camera != nullptr)
        {
            if(node->camera == camera)
            {
                //~ printf("camera[0] attached to node[%u]...\n", i);
                cgltf_camera_perspective *perspective= &camera->data.perspective;
                
                //~ if(perspective->has_aspect_ratio)
                    //~ printf("  aspect ratio %f\n", perspective->aspect_ratio);
                //~ printf("  yfov %f\n", perspective->yfov);
                //~ printf("  znear %f", perspective->znear);
                //~ if(perspective->has_zfar)
                    //~ printf(", zfar %f", perspective->zfar);
                //~ printf("\n");
                
                Transform projection= Perspective(perspective->yfov, perspective->aspect_ratio, perspective->znear, perspective->zfar);
                
                float model_matrix[16];
                cgltf_node_transform_world(node, model_matrix); // transformation globale
                
                Transform model;
                model.column_major(model_matrix);               // gltf organise les 16 floats par colonne...
                Transform view= Inverse(model);                 // view= inverse(model)
                
                std::vector<GLTFCamera> cameras;
                cameras.push_back( { perspective->yfov, perspective->aspect_ratio, perspective->znear, perspective->zfar, view, projection } );
                return cameras;
            }
        }
    }
    
    cgltf_free(data);
    return {};
}


std::vector<GLTFLight> read_gltf_lights( const char *filename )
{
    printf("loading glTF lights '%s'...\n", filename);
    
    cgltf_options options= { };
    cgltf_data *data= nullptr;
    cgltf_result code= cgltf_parse_file(&options, filename, &data);
    if(code != cgltf_result_success)
    {
        printf("[error] loading glTF mesh '%s'...\n", filename);
        return {};
    }
    
    if(cgltf_validate(data) != cgltf_result_success)
    {
        printf("[error] invalid glTF mesh '%s'...\n", filename);
        return {};
    }
    
    if(data->lights_count == 0)
    {
        printf("[warning] no lights...\n");
        return {};
    }
    
    std::vector<GLTFLight> lights;
    // retrouve les transformations associees aux sources
    for(unsigned i= 0; i < data->nodes_count; i++)
    {
        cgltf_node *node= &data->nodes[i];
        if(node->light != nullptr)
        {
            //~ int light_id= int(std::distance(data->lights, node->light));
            //~ printf("light[%u] attached to node[%u]...\n", light_id, i);
            
            // position de la source
            float model_matrix[16];
            cgltf_node_transform_world(node, model_matrix); // transformation globale
            
            Transform model;
            model.column_major(model_matrix);       // gltf organise les 16 floats par colonne...
            Point position= model(Origin());
            
            // proprietes de la source 
            cgltf_light *light= node->light;
            //~ printf("  position %f %f %f\n", position.x, position.y, position.z);
            //~ printf("  emission %f %f %f\n", light->color[0], light->color[1], light->color[2]);
            //~ printf("  intensity %f\n", light->intensity);
            
            lights.push_back( { position, Color(light->color[0], light->color[1], light->color[2]), light->intensity } );
        }
    }
    
    cgltf_free(data);
    return lights;
}


std::vector<GLTFMaterial> read_gltf_materials( const char *filename )
{
    printf("loading glTF materials '%s'...\n", filename);
    
    cgltf_options options= { };
    cgltf_data *data= nullptr;
    cgltf_result code= cgltf_parse_file(&options, filename, &data);
    if(code != cgltf_result_success)
    {
        printf("[error] loading glTF mesh '%s'...\n", filename);
        return {};
    }
    
    if(cgltf_validate(data) != cgltf_result_success)
    {
        printf("[error] invalid glTF mesh '%s'...\n", filename);
        return {};
    }
    
    if(data->materials_count ==0)
    {
        printf("[warning] no materials...\n");
        return {};
    }
    
    // materials
    std::vector<GLTFMaterial> materials(data->materials_count);
    for(unsigned i= 0; i < data->materials_count; i++)
    {
        cgltf_material *material= &data->materials[i];
        //~ printf("materials[%u] '%s'\n", i, material->name);
        
        GLTFMaterial m;
        m.color= Color(0.8, 0.8, 0.8, 1);
        m.metallic= 0;
        m.roughness= 1;
        m.color_texture= -1;
        m.metallic_roughness_texture= -1;
        m.occlusion_texture= -1;
        m.normal_texture= -1;
        m.emission_texture= -1;
        
        if(material->has_pbr_metallic_roughness)
        {
            cgltf_pbr_metallic_roughness *pbr= &material->pbr_metallic_roughness;
            //~ printf("  pbr metallic roughness\n");
            //~ printf("    base color %f %f %f %f\n", pbr->base_color_factor[0], pbr->base_color_factor[1], pbr->base_color_factor[2], pbr->base_color_factor[3]);
            //~ printf("      texture %d\n", pbr->base_color_texture.texture ? int(std::distance(data->images, pbr->base_color_texture.texture->image)) : -1);
            //~ printf("    metallic %f, roughness %f\n", pbr->metallic_factor, pbr->roughness_factor);
            //~ printf("      texture %d\n", pbr->metallic_roughness_texture.texture ? int(std::distance(data->images, pbr->metallic_roughness_texture.texture->image)) : -1);
            
            m.color= Color(pbr->base_color_factor[0], pbr->base_color_factor[1], pbr->base_color_factor[2], pbr->base_color_factor[3]);
            if(pbr->base_color_texture.texture && pbr->base_color_texture.texture->image)
                m.color_texture= int(std::distance(data->images, pbr->base_color_texture.texture->image));
            
            m.metallic= pbr->metallic_factor;
            m.roughness= pbr->roughness_factor;
            if(pbr->metallic_roughness_texture.texture && pbr->metallic_roughness_texture.texture->image)
                m.metallic_roughness_texture= int(std::distance(data->images, pbr->metallic_roughness_texture.texture->image));
        }
        //~ if(material->has_clearcoat)
            //~ printf("  clearcoat\n");
        //~ if(material->has_sheen)
            //~ printf("  sheen\n");
        
        if(material->normal_texture.texture && material->normal_texture.texture->image)
        {
            //~ printf("  normal texture %d\n", int(std::distance(data->images, material->normal_texture.texture->image)));
            m.normal_texture= int(std::distance(data->images, material->normal_texture.texture->image));
        }
        
        //~ printf("  emissive color %f %f %f\n", material->emissive_factor[0], material->emissive_factor[1], material->emissive_factor[2]);
        m.emission= Color(material->emissive_factor[0], material->emissive_factor[1], material->emissive_factor[2]);
        if(material->emissive_texture.texture && material->emissive_texture.texture->image)
        {
            //~ printf("    texture %d\n", int(std::distance(data->images, material->emissive_texture.texture->image)));
            m.emission_texture= int(std::distance(data->images, material->emissive_texture.texture->image));
        }
        
        materials.push_back(m);
    }
    
    cgltf_free(data);
    return materials;
}


std::vector<ImageData> read_gltf_images( const char *filename )
{
    printf("loading glTF images '%s'...\n", filename);
    
    cgltf_options options= { };
    cgltf_data *data= nullptr;
    cgltf_result code= cgltf_parse_file(&options, filename, &data);
    if(code != cgltf_result_success)
    {
        printf("[error] loading glTF mesh '%s'...\n", filename);
        return {};
    }
    
    if(cgltf_validate(data) != cgltf_result_success)
    {
        printf("[error] invalid glTF mesh '%s'...\n", filename);
        return {};
    }
    
    if(data->images_count == 0)
    {
        printf("[warning] no images...\n");
        return {};
    }
    
    std::vector<ImageData> images(data->images_count);
    
#pragma omp parallel for
    for(unsigned i= 0; i < data->images_count; i++)
    {
        //~ printf("[%u] %s\n", i, data->images[i].uri);
        if(data->images[i].uri)
        {
            std::string image_filename= pathname(filename) + std::string(data->images[i].uri);
            images[i]= read_image_data(image_filename.c_str());
        }
    }
    
    cgltf_free(data);
    return images;
}
