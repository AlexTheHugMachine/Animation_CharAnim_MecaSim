
//! \file viewer.cpp glTF viewer...

#include "cgltf.h"

//~ #include "gltf.h"
#include "wavefront.h"
#include "program.h"
#include "uniforms.h"

#include "files.h"
#include "image.h"
#include "image_io.h"
#include "texture.h"

#include "orbiter.h"
#include "draw.h"


//~ #include "app_camera.h"        // classe Application a deriver
#include "app_time.h"        // classe Application a deriver
#include "app.h"        // classe Application a deriver
#include "widgets.h"    // interface minimaliste

// utilitaire. creation d'une grille / repere.
Mesh make_grid( const int n= 10 )
{
    Mesh grid= Mesh(GL_LINES);
    
    // grille
    grid.color(White());
    for(int x= 0; x < n; x++)
    {
        float px= float(x) - float(n)/2 + .5f;
        grid.vertex(Point(px, 0, - float(n)/2 + .5f)); 
        grid.vertex(Point(px, 0, float(n)/2 - .5f));
    }

    for(int z= 0; z < n; z++)
    {
        float pz= float(z) - float(n)/2 + .5f;
        grid.vertex(Point(- float(n)/2 + .5f, 0, pz)); 
        grid.vertex(Point(float(n)/2 - .5f, 0, pz)); 
    }
    
    // axes XYZ
    grid.color(Red());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(1, .1, 0));
    
    grid.color(Green());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(0, 1, 0));
    
    grid.color(Blue());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(0, .1, 1));
    
    glLineWidth(2);
    
    return grid;
}

namespace gltf
{
    // parametres du glDrawElements()
    struct Primitives
    {
        GLuint vao;     // !! argh !! fusionner les primitives avec les memes attributs...
        GLenum mode;    // triangles
        GLenum type;    // GL_UNSIGNED_INT|SHORT|BYTE
        int offset;
        int count;
        int material_id;
        
        Point pmin;     // englobant... non transforme
        Point pmax;
    };
    
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
    
    struct Mesh
    {
        std::vector<Primitives> primitives;
    };
    
    struct Skeleton
    {
        std::string name;
        std::vector<int> nodes;
        std::vector<Transform> matrices;
    };
    
    struct Node
    {
        Transform model;
        Transform transform;    // composition des transforms individuelles pour l'animation
        
        vec4 transforms[3];
        // transforms[0]: translation
        // transforms[1]: rotation
        // transforms[2]: scale
        // transforms[3]: weights ??
        
        int mesh_id;
        int parent_id;
        int skin_id;
        
        bool has_trs;   // transformations TRS, eventuellement animees...
        bool has_animation;
    };
    
    struct Channel
    {
        std::vector<float> time;
        std::vector<float> values;
        int values_type;  // type de values 1: float, 2: vec2, 3: vec3, 4: vec4 etc.
        
        int target_id;      // node_id
        int transform_id;   // cf node::transforms
        int interpolation;  // nearest, linear, cubic
        int last_index;
    };
    
    struct Animation
    {
        std::string name;
        std::vector<Channel> channels;
    };
    
    // todo struct gltf::Model {};
}


class TP : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : App(1024, 640, 3, 3, 4) // MSAA 4
    //~ TP( ) : AppTime(1024, 640)
    {
        //~ SDL_GL_SetSwapInterval(1);
        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_SAMPLE_SHADING);
        glMinSampleShading(0);
        
        m_widgets= create_widgets();
    }
    
    // creation des objets de l'application
    int init( )
    {
        
        // charge un fichier glTF 2
        //~ const char *filename= "box.gltf";
        //~ const char *filename= "tutos/gltf/Robot.gltf";
        //~ const char *filename= "tutos/gltf/Fox/Fox.gltf";
        //~ const char *filename= "tutos/gltf/OrientationTest/OrientationTest.gltf";
        //~ const char *filename= "tutos/gltf/NormalTangent/NormalTangentTest.gltf";
        //~ const char *filename= "tutos/gltf/BoxAnimated/BoxAnimated.gltf";
        //~ const char *filename= "tutos/gltf/WaterBottle/WaterBottle.gltf";
        //~ const char *filename= "tutos/gltf/DamagedHelmet/DamagedHelmet.gltf";
        //~ const char *filename= "tutos/gltf/Drone/scene.gltf";
        const char *filename= "tutos/gltf/FlightHelmet/FlightHelmet.gltf";
        //~ const char *filename= "tutos/gltf/openGLNormal/normal.gltf";
        //~ const char *filename= "tutos/gltf/skinning.gltf";
        //~ const char *filename= "tutos/gltf/cube_scene.gltf";
        //~ const char *filename= "/home/jciehl/scenes/bistro_import/exterior.gltf";
        //~ const char *filename= "/home/jciehl/scenes/bistro-gltf/pack.gltf";
        //~ const char *filename= "/home/jciehl/scenes/quaternius/robot.gltf";
        //~ const char *filename= "/home/jciehl/Downloads/sintel/sintel.gltf";
        //~ const char *filename= "/home/jciehl/Downloads/drone-city/drone-city.gltf";
        //~ const char *filename= "/home/jciehl/scenes/sponza-intel/NewSponza_Main_Blender_glTF.gltf";
        //~ const char *filename= "/home/jciehl/scenes/RWT143/Lily-Maria.glb";
        //~ const char *filename= "/home/jciehl/scenes/RWT143/optimizer/export.gltf";
        //~ const char *filename= "/home/jciehl/Downloads/quaternius/rpg/Ranger.gltf";
        //~ const char *filename= "/home/jciehl/Downloads/quaternius/rpg/Rogue.gltf";
        //~ const char *filename= "/home/jciehl/scenes/splash3/splash.gltf";

    #if 0
        {
            Mesh mesh= read_gltf_mesh(filename);
            write_mesh(mesh, "export.obj");
            //~ write_materials(mesh.materials(), "export.mtl", pathname(filename).c_str());
            write_materials(mesh.materials(), "export.mtl");
            
            auto c= read_gltf_camera(filename);
            auto l= read_gltf_lights(filename);
            auto m= read_gltf_materials(filename);
            auto i= read_gltf_images(filename);
            //~ auto t= read_gltf_textures(filename);
            exit(0);
        }
    #endif
        
        
        // decrire un repere / grille 
        //~ m_repere= make_grid(100);
        m_repere= make_grid(25);
        
        m_program= read_program("tutos/gltf/viewer.glsl");
        program_print_errors(m_program);
        m_skinning_program= read_program("tutos/gltf/viewer_skinning.glsl");
        program_print_errors(m_skinning_program);
        {
            printf("loading glTF mesh '%s'...\n", filename);
            
            cgltf_options options= { };
            cgltf_data *data= nullptr;
            cgltf_result code= cgltf_parse_file(&options, filename, &data);
            if(code != cgltf_result_success)
            {
                printf("[error] loading glTF mesh '%s'...\n", filename);
                return -1;
            }
            
            if(cgltf_validate(data) != cgltf_result_success)
            {
                printf("[error] invalid glTF mesh '%s'...\n", filename);
                return -1;
            }
            printf("validating glTF mesh... done\n");
            
            printf("meshs %ld\n", data->meshes_count);
            printf("materials %ld\n", data->materials_count);
            printf("accessors %ld\n", data->accessors_count);
            printf("buffer views %ld\n", data->buffer_views_count);
            printf("buffers %ld\n", data->buffers_count);
            {
                //~ for(unsigned i= 0; i < data->buffers_count; i++)
                //~ {
                    //~ cgltf_buffer *buffer= &data->buffers[i];
                    //~ assert(buffer);
                    //~ printf("  buffer %d:\n", i);
                    //~ printf("    name '%s'\n", buffer->name);
                    //~ printf("    size %ld\n", buffer->size);
                //~ }
            }
            
            printf("cameras %ld\n", data->cameras_count);
            printf("lights %ld\n", data->lights_count);
            printf("nodes %ld\n", data->nodes_count);
            printf("scenes %ld\n", data->scenes_count);
            printf("animations %ld\n", data->animations_count);
            printf("skins %ld\n", data->skins_count);
            
            // load buffers
            //~ printf("loading buffers...\n");
            code= cgltf_load_buffers(&options, data, filename);
            if(code != cgltf_result_success)
            {
                printf("[error] loading glTF buffers...\n");
                exit(1);
            }
            printf("loading buffers... done\n");
            
            // load images
            printf("images %ld...\n", data->images_count);
            {
                m_textures.resize(data->images_count);
                std::vector<ImageData> images(data->images_count);
                
            #pragma omp parallel for
                for(unsigned i= 0; i < data->images_count; i++)
                {
                    //~ printf("%u: uri '%s', buffer_view %p\n", i, data->images[i].uri, data->images[i].buffer_view);
                    
                    // charger les images en parallele
                    if(data->images[i].uri)
                    {
                        std::string texture_filename= pathname(filename) + std::string(data->images[i].uri);
                        ImageData image= read_image_data(texture_filename.c_str());
                        
                        // argh !! gltf inverse la convention uv, l'origine est en haut a gauche, au lieu de en bas a gauche pour openGL...
                        images[i]= flipY(image);
                        //~ images[i]= image;
                    }
                    
                    /* \todo si l'image est stockee dans un buffer, utiliser SDL_RWFromConstMem + IMG_LoadPNG_RW / IMG_LoadJPG_RW
                        cf https://wiki.libsdl.org/SDL_RWFromConstMem
                        + https://www.libsdl.org/projects/SDL_image/docs/SDL_image.html#SEC24
                     */
                }

                // texture par defaut, valeur constante 1
                {
                    Image white(16, 16, White());
                    m_white_texture= make_texture(0, white, GL_RGBA);
                }
                // texture par defaut, valeur constante (0, 0, 1), cf normal maps
                {
                    Image blue(16, 16, Blue());
                    m_blue_texture= make_texture(0, blue, GL_RGBA);
                }
                // texture par defaut, valeur constante 0
                {
                    Image black(16, 16, Black());
                    m_black_texture= make_texture(0, black, GL_RGBA);
                }
            
                // creer les textures sur le thread principal / opengl
                for(unsigned i= 0; i < images.size(); i++)
                {
                    m_textures[i]= make_texture(0, images[i]);
                    if(m_textures[i] == 0)
                        m_textures[i]= m_white_texture;
                    
                    // repetition par defaut
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    //~ glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, float(4));
                }
            }
            
            // materials
            printf("materials %ld...\n", data->materials_count);
            for(unsigned i= 0; i < data->materials_count; i++)
            {
                cgltf_material *material= &data->materials[i];
                
                gltf::Material m;
                m.color= Color(0.8, 0.8, 0.8, 1);
                m.color_texture= -1;
                m.metallic_roughness_texture= -1;
                m.occlusion_texture= -1;
                m.normal_texture= -1;
                m.emission= Black();
                m.emission_texture= -1;
                
                printf("[%u] '%s'\n", i, material->name);
                if(material->has_pbr_metallic_roughness)
                {
                    printf("  pbr metallic roughness\n");
                    cgltf_pbr_metallic_roughness *pbr= &material->pbr_metallic_roughness;
                    //~ printf("    base color %f %f %f %f\n", pbr->base_color_factor[0], pbr->base_color_factor[1], pbr->base_color_factor[2], pbr->base_color_factor[3]);
                    //~ printf("      texture %d\n", pbr->base_color_texture.texture ? int(std::distance(data->images, pbr->base_color_texture.texture->image)) : -1);
                    if(pbr->base_color_texture.texture)
                    {
                        //~ printf("      texcoord %d\n", pbr->base_color_texture.texcoord);
                        if(pbr->base_color_texture.has_transform)
                            printf("      transform !!\n");
                        
                    #if 0
                        cgltf_sampler *sampler= pbr->base_color_texture.texture->sampler;
                        const char *wrap_s= "";
                        switch(sampler->wrap_s)
                        {
                            case GL_CLAMP_TO_EDGE: wrap_s= "clamp_to_edge"; break;
                            case GL_MIRRORED_REPEAT: wrap_s= "mirrored repeat"; break;
                            case GL_REPEAT: wrap_s= "repeat"; break;
                        }
                        const char *wrap_t= "";
                        switch(sampler->wrap_t)
                        {
                            case GL_CLAMP_TO_EDGE: wrap_t= "clamp_to_edge"; break;
                            case GL_MIRRORED_REPEAT: wrap_t= "mirrored repeat"; break;
                            case GL_REPEAT: wrap_t= "repeat"; break;
                        }
                        
                        const char *filter_min= "";
                        switch(sampler->min_filter)
                        {
                            case GL_NEAREST: filter_min= "nearest"; break;
                            case GL_LINEAR: filter_min= "linear"; break;
                            case GL_NEAREST_MIPMAP_NEAREST: filter_min= "nearest mipmap nearest"; break;
                            case GL_NEAREST_MIPMAP_LINEAR: filter_min= "nearest mipmap linear"; break;
                            case GL_LINEAR_MIPMAP_NEAREST: filter_min= "linear mipmap linear"; break;
                            case GL_LINEAR_MIPMAP_LINEAR: filter_min= "linear mipmap linear"; break;
                        }
                        
                        const char *filter_mag= "";
                        switch(sampler->mag_filter)
                        {
                            case GL_NEAREST: filter_mag= "nearest"; break;
                            case GL_LINEAR: filter_mag= "linear"; break;
                        }
                        
                        printf("    sampler wrap (%s, %s), filter (%s, %s)\n", wrap_s, wrap_t, filter_min, filter_mag);
                    #endif
                    }
                    
                    m.color= Color(pbr->base_color_factor[0], pbr->base_color_factor[1], pbr->base_color_factor[2], pbr->base_color_factor[3]);
                    if(pbr->base_color_texture.texture && pbr->base_color_texture.texture->image)
                        m.color_texture= int(std::distance(data->images, pbr->base_color_texture.texture->image));
                    
                    m.metallic= pbr->metallic_factor;
                    m.roughness= pbr->roughness_factor;
                    if(pbr->metallic_roughness_texture.texture && pbr->metallic_roughness_texture.texture->image)
                        m.metallic_roughness_texture= int(std::distance(data->images, pbr->metallic_roughness_texture.texture->image));
                    
                    printf("    base color %f %f %f %f\n", m.color.r, m.color.g, m.color.b, m.color.a);
                    printf("      texture %d\n", pbr->base_color_texture.texture ? int(std::distance(data->images, pbr->base_color_texture.texture->image)) : -1);
                    printf("    metallic %f, roughness %f\n", pbr->metallic_factor, pbr->roughness_factor);
                    printf("      texture %d\n", pbr->metallic_roughness_texture.texture ? int(std::distance(data->images, pbr->metallic_roughness_texture.texture->image)) : -1);
                    
                }
                if(material->has_clearcoat)
                    printf("  clearcoat\n");
                if(material->has_sheen)
                    printf("  sheen\n");
                if(material->normal_texture.texture && material->normal_texture.texture->image)
                {
                    //~ printf("  normal texture %d\n", int(std::distance(data->images, material->normal_texture.texture->image)));
                    m.normal_texture= int(std::distance(data->images, material->normal_texture.texture->image));
                }
                if(material->occlusion_texture.texture && material->occlusion_texture.texture->image)
                {
                    //~ printf("  occlusion texture %d\n", int(std::distance(data->images, material->occlusion_texture.texture->image)));
                    m.occlusion_texture= int(std::distance(data->images, material->occlusion_texture.texture->image));
                }
                
                //~ printf("  emissive color %f %f %f\n", material->emissive_factor[0], material->emissive_factor[1], material->emissive_factor[2]);
                m.emission= Color(material->emissive_factor[0], material->emissive_factor[1], material->emissive_factor[2]);
                if(material->emissive_texture.texture && material->emissive_texture.texture->image)
                {
                    //~ printf("    texture %d\n", int(std::distance(data->images, material->emissive_texture.texture->image)));
                    m.emission_texture= int(std::distance(data->images, material->emissive_texture.texture->image));
                }
                
                if(material->has_pbr_specular_glossiness)
                    printf("  pbr  specular glossiness\n");
                    
                m_materials.push_back(m);
            }

            // ressources openGL
            for(unsigned i= 0; i < data->buffers_count; i++)
            {
                GLuint buffer= 0;
                glGenBuffers(1, &buffer);
                
                glBindBuffer(GL_ARRAY_BUFFER, buffer);
                glBufferData(GL_ARRAY_BUFFER, data->buffers[i].size, data->buffers[i].data, GL_STATIC_DRAW);
                
                m_buffers.push_back(buffer);
            }
            
            // meshes / buffers
            for(unsigned i= 0; i < data->meshes_count; i++)
            {
                gltf::Mesh mesh= init_mesh(data, &data->meshes[i]);
                m_meshes.push_back(mesh);
            }
            printf("creating meshes... done\n");
            
            // scene / nodes
            assert(data->scene);
            init_scene(data, data->scene);
            printf("creating scene... done\n");
            
            // animations
            for(unsigned i= 0; i < data->animations_count; i++)
            {
                gltf::Animation a= init_animation(data, &data->animations[i]);
                m_animations.push_back(a);
            }
            printf("creating animations... done\n");
            
            // skins
            for(unsigned i= 0; i < data->skins_count; i++)
            {
                printf("skin '%s', %d nodes\n", data->skins[i].name, int(data->skins[i].joints_count));
                gltf::Skeleton s= init_skeleton(data, &data->skins[i]);
                m_skins.push_back(s);
            }
            printf("creating skins... done\n");
            
            cgltf_free(data);
        }
        
        // si l'objet est "gros", il faut regler la camera pour l'observer entierement :
    #if 1
        // todo transformations des nodes / meshes !!
        // recuperer les points extremes de l'objet (son englobant)
        Point pmin= Point(FLT_MAX, FLT_MAX, FLT_MAX);
        Point pmax= Point(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for(unsigned i= 0; i < m_meshes.size(); i++)
        {
            const gltf::Mesh& m= m_meshes[i];
            for(unsigned k= 0; k < m.primitives.size(); k++)
            {
                pmin.x= std::min(pmin.x, m.primitives[k].pmin.x);
                pmin.y= std::min(pmin.y, m.primitives[k].pmin.y);
                pmin.z= std::min(pmin.z, m.primitives[k].pmin.z);
                pmax.x= std::max(pmax.x, m.primitives[k].pmax.x);
                pmax.y= std::max(pmax.y, m.primitives[k].pmax.y);
                pmax.z= std::max(pmax.z, m.primitives[k].pmax.z);
            }
        }
    #else
    
        Point pmin, pmax;
        m_repere.bounds(pmin, pmax);
    #endif        
        // parametrer la camera de l'application, renvoyee par la fonction camera()
        m_camera.projection(window_width(), window_height(), 45);
        m_camera.lookat(pmin, pmax);
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest
        
        return 0;   // ras, pas d'erreur
        //~ return -1;   // ras, pas d'erreur
    }
    
    void print_accessor( cgltf_data *data, cgltf_accessor *accessor )
    {
        const char *type= nullptr;
        switch(accessor->type)
        {
            case cgltf_type_scalar: type= "scalar"; break;
            case cgltf_type_vec2: type= "vec2"; break;
            case cgltf_type_vec3: type= "vec3"; break;
            case cgltf_type_vec4: type= "vec4"; break;
            case cgltf_type_mat2: type= "mat2"; break;
            case cgltf_type_mat3: type= "mat3"; break;
            case cgltf_type_mat4: type= "mat4"; break;
            default: type= "<unknown>";
        }
        
        const char *ctype= nullptr;
        switch(accessor->component_type)
        {
            case cgltf_component_type_r_8: ctype= "int8"; break;
            case cgltf_component_type_r_8u: ctype= "uint8"; break;
            case cgltf_component_type_r_16: ctype= "int16"; break;
            case cgltf_component_type_r_16u: ctype= "uint16"; break;
            case cgltf_component_type_r_32u: ctype= "uint32"; break;
            case cgltf_component_type_r_32f: ctype= "float"; break;
            default: ctype= "<unknown>";
        }
        
        printf("        %s (%s)\n", type, ctype);
        
        cgltf_buffer_view *view= accessor->buffer_view;
        printf("        count %ld offset %ld stride %ld, buffer view size %ld, offset %ld\n", accessor->count, accessor->offset, accessor->stride, view->size, view->offset);
        if(view->data)
            printf("          buffer_view data !\n");
        printf("        buffer index %d\n", int(std::distance(data->buffers, view->buffer)));
    }
    
    void print_animation_sampler( cgltf_data *data, cgltf_animation_sampler *sampler )
    {
        // cgltf_accessor_unpack_floats(const cgltf_accessor* accessor, cgltf_float* out, cgltf_size float_count);    
        std::vector<float> input(cgltf_accessor_unpack_floats(sampler->input, nullptr, 0));
        cgltf_accessor_unpack_floats(sampler->input, input.data(), input.size());
        
        printf("    input %d floats, sampler %d\n", int(sampler->input->count), int(std::distance(data->accessors, sampler->input)));
        //~ for(unsigned i= 0; i < input.size(); i++)
            //~ printf("%f ", input[i]);
        //~ printf("\n");
        
        int type= 0;
        switch(sampler->output->type)
        {
            case cgltf_type_scalar: type= 1; break;
            case cgltf_type_vec2: type= 2; break;
            case cgltf_type_vec3: type= 3; break;
            case cgltf_type_vec4: type= 4; break;
            default: type= 0;
        }
        
        std::vector<float> output(cgltf_accessor_unpack_floats(sampler->output, nullptr, 0));
        cgltf_accessor_unpack_floats(sampler->output, output.data(), output.size());
        printf("    output %dx%d floats, sampler %d\n", int(sampler->output->count), type, int(std::distance(data->accessors, sampler->output)));
        //~ for(unsigned i= 0; i < output.size(); i++)
            //~ printf("%f ", output[i]);
        //~ printf("\n");
        
    }
    
    void print_animation( cgltf_data *data, cgltf_animation *animation )
    {
        printf("animation '%s' %d channels, %d samplers\n", animation->name, int(animation->channels_count), int(animation->samplers_count));
        
        for(unsigned i= 0; i < animation->channels_count; i++)
        {
            cgltf_animation_channel *channel= &animation->channels[i];
            const char *type= nullptr;
            switch(channel->target_path)
            {
                case cgltf_animation_path_type_translation: type= "translation"; break;
                case cgltf_animation_path_type_rotation: type= "rotation"; break;
                case cgltf_animation_path_type_scale: type= "scale"; break;
                case cgltf_animation_path_type_weights: type= "weights"; break;
                default: type= "<unknown>";
            }
            
            int node_id= std::distance(data->nodes, channel->target_node);
            printf("  channel %d: type '%s' target node %d '%s'\n", i, type, node_id, channel->target_node->name);
            
            print_animation_sampler(data, channel->sampler);
        }
    }
    
    gltf::Channel init_channel( cgltf_data *data, cgltf_animation_channel *channel )
    {
        gltf::Channel c;
        
        int type= -1;
        switch(channel->target_path)
        {
            case cgltf_animation_path_type_translation: type= 0; break;
            case cgltf_animation_path_type_rotation: type= 1; break;
            case cgltf_animation_path_type_scale: type= 2; break;
            //~ case cgltf_animation_path_type_weights: type= 3; break;
            default: type= -1;
        }
        
        if(type == -1)
            return c;
            
        //~ assert(type != -1);
        c.transform_id= type;
        c.target_id= std::distance(data->nodes, channel->target_node);
        
        //~ printf("channel node %d\n", c.target_id);
        
        cgltf_animation_sampler *sampler= channel->sampler;
        c.time.resize(cgltf_accessor_unpack_floats(sampler->input, nullptr, 0));
        cgltf_accessor_unpack_floats(sampler->input, c.time.data(), c.time.size());
        
        int vtype= 0;
        switch(sampler->output->type)
        {
            case cgltf_type_scalar: vtype= 1; break;
            case cgltf_type_vec2: vtype= 2; break;
            case cgltf_type_vec3: vtype= 3; break;
            case cgltf_type_vec4: vtype= 4; break;
            default: vtype= 0;
        }
        
        assert(vtype != 0);
        c.values_type= vtype;
        c.values.resize(cgltf_accessor_unpack_floats(sampler->output, nullptr, 0));
        cgltf_accessor_unpack_floats(sampler->output, c.values.data(), c.values.size());
        
        return c;
    }
    
    gltf::Animation init_animation( cgltf_data *data, cgltf_animation *animation )
    {
        gltf::Animation a;
        if(animation->name)
            a.name= animation->name;
        
        for(unsigned i= 0; i < animation->channels_count; i++)
        {
            cgltf_animation_channel *channel= &animation->channels[i];
            gltf::Channel c= init_channel(data, channel);
            a.channels.push_back(c);
        }
        printf("init animation '%s', %d channels\n", animation->name, int(a.channels.size()));
        
        return a;
    }
    
    gltf::Primitives init_primitives( cgltf_data *data, cgltf_primitive *primitive )
    {
        gltf::Primitives p;
        
        //~ const char *type= nullptr;
        //~ switch(primitive->type)
        //~ {
            //~ case cgltf_primitive_type_points: type= "points"; break;
            //~ case cgltf_primitive_type_lines: type= "lines"; break;
            //~ case cgltf_primitive_type_line_loop: type= "line_loop"; break;
            //~ case cgltf_primitive_type_line_strip: type= "line_strip"; break;
            //~ case cgltf_primitive_type_triangles: type= "triangles"; break;
            //~ case cgltf_primitive_type_triangle_strip: type= "triangle_strip"; break;
            //~ case cgltf_primitive_type_triangle_fan: type= "triangle_fan"; break;
            //~ default: type= "<unknown>";
        //~ }
        
        //~ if(primitive->type != cgltf_primitive_type_triangles)
            //~ printf("  init primitives '%s'\n", type);
        
        assert(primitive->type == cgltf_primitive_type_triangles);
        p.mode= GL_TRIANGLES;
        
        p.material_id= -1;
        if(primitive->material)
            p.material_id= int(std::distance(data->materials, primitive->material));
        
        glGenVertexArrays(1, &p.vao);
        glBindVertexArray(p.vao);
        
        p.type= GL_NONE;
        
        if(primitive->indices)
        {
            // index buffer
            //~ printf("    indices %ld\n", primitive->indices->count);
            //~ print_accessor(data, primitive->indices);
            
            switch(primitive->indices->component_type)
            {
                case cgltf_component_type_r_8u: p.type= GL_UNSIGNED_BYTE; break;
                case cgltf_component_type_r_16u: p.type= GL_UNSIGNED_SHORT; break;
                case cgltf_component_type_r_32u: p.type= GL_UNSIGNED_INT; break;
                default: p.type= GL_NONE;
            }
            assert(p.type != GL_NONE);
            
            p.count= primitive->indices->count;
            p.offset= primitive->indices->offset + primitive->indices->buffer_view->offset;
            assert(primitive->indices->buffer_view->data == nullptr);
            unsigned buffer= unsigned(std::distance(data->buffers, primitive->indices->buffer_view->buffer));
            assert(buffer < m_buffers.size());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[buffer]);
        }
        
        // attributes
        bool has_normal= false;
        bool has_tangent= false;
        
        //~ printf("    attributes %ld\n", primitive->attributes_count);
        p.pmin= Point(FLT_MAX, FLT_MAX, FLT_MAX);
        p.pmax= Point(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for(unsigned i= 0; i < primitive->attributes_count; i++)
        {
            cgltf_attribute *attribute= &primitive->attributes[i];
            
            const char *type= nullptr;
            switch(attribute->type)
            {
                case cgltf_attribute_type_position: type= "position"; break;
                case cgltf_attribute_type_normal: type= "normal"; break;
                case cgltf_attribute_type_tangent: type= "tangent"; break;
                case cgltf_attribute_type_texcoord: type= "texcoord"; break;
                case cgltf_attribute_type_color: type= "color"; break;
                case cgltf_attribute_type_joints: type= "joints"; break;
                case cgltf_attribute_type_weights: type= "weights"; break;
                default: type= "<unknown>";
            }
            //~ printf("      %s:%d, name '%s'\n", type, attribute->index, attribute->name);
            //~ print_accessor(data, attribute->data);
            
            if(attribute->type == cgltf_attribute_type_position)
            {
                //~ printf("      %s:%d, name '%s'\n", type, attribute->index, attribute->name);
                //~ print_accessor(data, attribute->data);
                
                assert(attribute->data->type == cgltf_type_vec3);
                assert(attribute->data->component_type == cgltf_component_type_r_32f);
                
                assert(attribute->data->has_min &&attribute->data->has_max);
                p.pmin.x= std::min(p.pmin.x, attribute->data->min[0]);
                p.pmin.y= std::min(p.pmin.y, attribute->data->min[1]);
                p.pmin.z= std::min(p.pmin.z, attribute->data->min[2]);
                p.pmax.x= std::max(p.pmax.x, attribute->data->max[0]);
                p.pmax.y= std::max(p.pmax.y, attribute->data->max[1]);
                p.pmax.z= std::max(p.pmax.z, attribute->data->max[2]);
                
                unsigned buffer= unsigned(std::distance(data->buffers, attribute->data->buffer_view->buffer));
                assert(attribute->data->buffer_view->data == nullptr);
                assert(buffer < m_buffers.size());
                size_t offset= attribute->data->buffer_view->offset + attribute->data->offset;
                unsigned stride= attribute->data->stride;
                
                glBindBuffer(GL_ARRAY_BUFFER, m_buffers[buffer]);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void *) offset);        // attribut 0
                glEnableVertexAttribArray(0);
                
                if(primitive->indices == nullptr)
                    p.count= attribute->data->count;
            }
            if(attribute->type == cgltf_attribute_type_texcoord)
            {
                //~ printf("      %s:%d, name '%s'\n", type, attribute->index, attribute->name);
                //~ print_accessor(data, attribute->data);
                
                assert(attribute->data->type == cgltf_type_vec2);
                assert(attribute->data->component_type == cgltf_component_type_r_32f);
                
                unsigned buffer= unsigned(std::distance(data->buffers, attribute->data->buffer_view->buffer));
                assert(attribute->data->buffer_view->data == nullptr);
                assert(buffer < m_buffers.size());
                size_t offset= attribute->data->buffer_view->offset + attribute->data->offset;
                unsigned stride= attribute->data->stride;
                
                glBindBuffer(GL_ARRAY_BUFFER, m_buffers[buffer]);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void *) offset);        // attribut 1
                glEnableVertexAttribArray(1);
            }
            if(attribute->type == cgltf_attribute_type_normal)
            {
                //~ printf("      %s:%d, name '%s'\n", type, attribute->index, attribute->name);
                //~ print_accessor(data, attribute->data);
                
                assert(attribute->data->type == cgltf_type_vec3);
                assert(attribute->data->component_type == cgltf_component_type_r_32f);
                
                has_normal= true;
                unsigned buffer= unsigned(std::distance(data->buffers, attribute->data->buffer_view->buffer));
                assert(attribute->data->buffer_view->data == nullptr);
                assert(buffer < m_buffers.size());
                size_t offset= attribute->data->buffer_view->offset + attribute->data->offset;
                unsigned stride= attribute->data->stride;
                
                glBindBuffer(GL_ARRAY_BUFFER, m_buffers[buffer]);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (const void *) offset);        // attribut 2
                glEnableVertexAttribArray(2);
            }
            if(attribute->type == cgltf_attribute_type_tangent)
            {
                //~ printf("      %s:%d, name '%s'\n", type, attribute->index, attribute->name);
                //~ print_accessor(data, attribute->data);
                
                //~ assert(attribute->data->type == cgltf_type_vec4);   // x,y,z + sign
                //~ assert(attribute->data->component_type == cgltf_component_type_r_32f);
                
                //~ has_tangent= true;
            }
            
            if(attribute->type == cgltf_attribute_type_joints)
            {
                //~ printf("      %s:%d, name '%s'\n", type, attribute->index, attribute->name);
                //~ print_accessor(data, attribute->data);
                
                assert(attribute->data->type == cgltf_type_vec4);
                //~ assert(attribute->data->component_type == cgltf_component_type_r_8u);
                
                unsigned buffer= unsigned(std::distance(data->buffers, attribute->data->buffer_view->buffer));
                assert(attribute->data->buffer_view->data == nullptr);
                assert(buffer < m_buffers.size());
                size_t offset= attribute->data->buffer_view->offset + attribute->data->offset;
                unsigned stride= attribute->data->stride;
                
                glBindBuffer(GL_ARRAY_BUFFER, m_buffers[buffer]);
                if(attribute->data->component_type == cgltf_component_type_r_8u)
                    glVertexAttribIPointer(3, 4, GL_UNSIGNED_BYTE, stride, (const void *) offset);        // attribut 3
                else if(attribute->data->component_type == cgltf_component_type_r_16u)
                    glVertexAttribIPointer(3, 4, GL_UNSIGNED_SHORT, stride, (const void *) offset);        // attribut 3
                else if(attribute->data->component_type == cgltf_component_type_r_32u)
                    glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, stride, (const void *) offset);        // attribut 3
                else
                    printf("[error] unknown joint index type...\n");
                
                glEnableVertexAttribArray(3);
            }
            if(attribute->type == cgltf_attribute_type_weights)
            {
                //~ printf("      %s:%d, name '%s'\n", type, attribute->index, attribute->name);
                //~ print_accessor(data, attribute->data);
                
                assert(attribute->data->type == cgltf_type_vec4);
                assert(attribute->data->component_type == cgltf_component_type_r_32f);
                
                unsigned buffer= unsigned(std::distance(data->buffers, attribute->data->buffer_view->buffer));
                assert(attribute->data->buffer_view->data == nullptr);
                assert(buffer < m_buffers.size());
                size_t offset= attribute->data->buffer_view->offset + attribute->data->offset;
                unsigned stride= attribute->data->stride;
                
                glBindBuffer(GL_ARRAY_BUFFER, m_buffers[buffer]);
                glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offset);        // attribut 4
                glEnableVertexAttribArray(4);
            }
        }
        
    #if 0
        if(p.material_id != -1)
        {
            assert(p.material_id < int(m_materials.size()));
            gltf::Material *material= &m_materials[p.material_id];
            
            if(has_normal == false)
                printf("  [warning] shading but no normals...\n");
            if(material->normal_texture != -1 && has_tangent == false)
                printf("  [warning] normal map but no tangents...\n");
        }
    #endif
        
        assert(p.count != 0);
        return p;
    }
    
    gltf::Mesh init_mesh( cgltf_data *data, cgltf_mesh *mesh )
    {
        gltf::Mesh m;
        for(unsigned i= 0; i < mesh->primitives_count; i++)
        {
            gltf::Primitives p= init_primitives(data, &mesh->primitives[i]);
            m.primitives.push_back(p);
        }
        
        return m;
    }
    
    gltf::Skeleton init_skeleton( cgltf_data *data, cgltf_skin *skin )
    {
        //~ print_accessor(data, skin->inverse_bind_matrices);
        
        // recupere les matrices
        std::vector<float> matrices;
        matrices.resize(cgltf_accessor_unpack_floats(skin->inverse_bind_matrices, nullptr, 0));
        cgltf_accessor_unpack_floats(skin->inverse_bind_matrices, matrices.data(), matrices.size());
        
        gltf::Skeleton s;
        for(unsigned i= 0; i < skin->joints_count; i++)
        {
            s.nodes.push_back(std::distance(data->nodes, skin->joints[i]));
            
            Transform m;
            m.column_major(&matrices[16*i]);
            s.matrices.push_back(m);
        }
        
        return s;
    }
    
    
    // parcours les noeuds de la scene et trouve tous les meshs + leurs transformations
    gltf::Node init_node( cgltf_data *data, cgltf_node *node )
    {
        gltf::Node n;
        
        float matrix[16];
        cgltf_node_transform_local(node, matrix);   // transformation dans le repere du parent
        n.transform.column_major(matrix);   // gltf organise les 16 floats par colonne...

        // transformations animees...
        n.has_trs= !node->has_matrix;   // transformations TRS par defaut, si pas de matrice...
        n.has_animation= false;
        
        n.transforms[0]= vec4(0, 0, 0, 0);  // pas de translation
        if(node->has_translation)
            n.transforms[0]= vec4(node->translation[0], node->translation[1], node->translation[2], 0);
        n.transforms[1]= vec4(0, 0, 0, 1);  // pas de rotation / unit quaternion
        if(node->has_rotation)
            n.transforms[1]= vec4(node->rotation[0], node->rotation[1], node->rotation[2], node->rotation[3]);
        n.transforms[2]= vec4(1, 1, 1, 0);  // pas de scale
        if(node->has_scale)
            n.transforms[2]= vec4(node->scale[0], node->scale[1], node->scale[2], 0);
        
        cgltf_node_transform_world(node, matrix);   // transformation globale
        n.model.column_major(matrix);   // gltf organise les 16 floats par colonne...
        
        n.mesh_id= -1;
        if(node->mesh)
            n.mesh_id= int(std::distance(data->meshes, node->mesh));
        
        n.parent_id= -1;
        if(node->parent)
            n.parent_id= int(std::distance(data->nodes, node->parent));
        
        n.skin_id= -1;
        if(node->skin)
            n.skin_id= int(std::distance(data->skins, node->skin));
        
        return n;
    }
    
    void init_scene( cgltf_data *data, cgltf_scene *scene )
    {
        printf("init scene '%s'\n", scene->name);
        
        // duplique la hierarchie de noeuds, cf animation des transformations des noeuds
        for(unsigned i= 0; i < data->nodes_count; i++)
        {
            gltf::Node n= init_node(data, &data->nodes[i]);
            m_nodes.push_back(n);
        }
        
        // todo stocke les noeuds / racines de la scene... lorsqu'il y a plusieurs scenes...
    }
    
    
    void play_channel( gltf::Channel& channel, const float app_time )
    {
        if(channel.time.empty())
            return;
        
        assert(channel.time.size() > 0);
        float time= std::fmod(app_time, channel.time.back());
        
        int index= -1;
        for(unsigned i= 0; i < channel.time.size(); i++)
        {
            if(channel.time[i] >= time)
            {
                index= i;
                break;
            }
        }
        
        if(index == -1)
            return;
        
        float t_previous= 0;
        int previous= index -1;
        if(previous >= 0)
            t_previous= channel.time[previous];
        //~ if(previous < 0)
            //~ previous= 0;
        
        //~ assert(channel.time[previous] <= time);
        assert(channel.time[index] >= time);
        //~ float t= (time - channel.time[previous]) / (channel.time[index] - channel.time[previous]);
        float t= (time - t_previous) / (channel.time[index] - t_previous);
        if(t < 0) t= 0;
        if(t > 1) t= 1;
        
        //~ printf("app %f loop %f, time %f, t %f, previous %d %f, current index %d %f\n", app_time, channel.time.back(), 
            //~ time, t,
            //~ previous, channel.time[previous], index, channel.time[index]);
        
        vec4 a;
        switch(channel.transform_id)
        {
            case 0: a= vec4(0,0,0,0); break;    // translation
            case 1: a= vec4(0,0,0,1); break;    // rotation
            case 2: a= vec4(1,1,1,1); break;    // scale
            default: break;
        }
        
        if(previous >= 0)
        {
            a.x= channel.values[channel.values_type * previous];
            if(channel.values_type > 1)
                a.y= channel.values[channel.values_type * previous +1];
            if(channel.values_type > 2)
                a.z= channel.values[channel.values_type * previous +2];
            if(channel.values_type > 3)
                a.w= channel.values[channel.values_type * previous +3];
        }
        
        vec4 b;
        b.x= channel.values[channel.values_type * index];
        if(channel.values_type > 1)
            b.y= channel.values[channel.values_type * index +1];
        if(channel.values_type > 2)
            b.z= channel.values[channel.values_type * index +2];
        if(channel.values_type > 3)
            b.w= channel.values[channel.values_type * index +3];
        
        vec4 m;
        // interpolation des rotations / quaternions... 
        if(channel.transform_id == 1)
        {
            // [w, v(x, y, z)] , cf "For the "rotation" property, the values are a quaternion in the order (x, y, z, w), where w is the scalar."
            float d= a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
            if(d < 0)
                a= vec4(-a.x, -a.y, -a.z, -a.w);
            
            // todo slerp...
            m.x= a.x + t * (b.x - a.x);
            m.y= a.y + t * (b.y - a.y);
            m.z= a.z + t * (b.z - a.z);
            m.w= a.w + t * (b.w - a.w);
            
            // normalisation
            float n= std::sqrt(m.x*m.x + m.y*m.y + m.z*m.z + m.w*m.w);
            m.x= m.x / n; m.y= m.y / n; m.z= m.z / n; m.w= m.w / n;
            
            // c'est moche, mais il faut normaliser le resultat de l'interpolation des quaternions...
        }
        else
        {
            // interpolation des autres transformations : translation / scale
            m.x= a.x + t * (b.x - a.x);
            m.y= a.y + t * (b.y - a.y);
            m.z= a.z + t * (b.z - a.z);
            m.w= a.w + t * (b.w - a.w);
        }
        
        assert(channel.target_id < int(m_nodes.size()));
        gltf::Node &target= m_nodes[channel.target_id];
        
        assert(target.has_trs); // si la transformation du noeud est definie directement par une matrice et pas les transformations TRS, ca ne marchera pas...
        assert(channel.transform_id >= 0 && channel.transform_id < 3);
        target.transforms[channel.transform_id]= m;
        target.has_animation= true; // marque le noeud
    }
    
    void play_animation( const int id, const float time )
    {
        if(id < 0 || id >= int(m_animations.size()))
            return;
        
        gltf::Animation& animation= m_animations[id];
        //~ printf("play '%s' time %f\n", animation.name.c_str(), time);
        
        for(unsigned i= 0; i < animation.channels.size(); i++)
            play_channel(animation.channels[i], time);
    }
    
    void update_transforms( )
    {
        // etape 1: recalcul des transformations TRS des noeuds
        for(unsigned i= 0; i < m_nodes.size(); i++)
        {
            gltf::Node& node= m_nodes[i];
            if(node.has_trs == false)
                continue;   // rien a recalculer
            
            float tx = node.transforms[0].x;
            float ty = node.transforms[0].y;
            float tz = node.transforms[0].z;
            
            float qx = node.transforms[1].x;
            float qy = node.transforms[1].y;
            float qz = node.transforms[1].z;
            float qw = node.transforms[1].w;
            
            float sx = node.transforms[2].x;
            float sy = node.transforms[2].y;
            float sz = node.transforms[2].z;
            
            // composition des transformations TRS, cf cgltf_node_transform_local()
            Transform m;
            
            // construit la matrice colonne par colonne... 
            // c0
            m.m[0][0]= (1 - 2 * qy*qy - 2 * qz*qz) * sx;
            m.m[1][0]= (2 * qx*qy + 2 * qz*qw) * sx;
            m.m[2][0]= (2 * qx*qz - 2 * qy*qw) * sx;
            m.m[3][0]= 0.f;
            
            // c1
            m.m[0][1]= (2 * qx*qy - 2 * qz*qw) * sy;
            m.m[1][1]= (1 - 2 * qx*qx - 2 * qz*qz) * sy;
            m.m[2][1]= (2 * qy*qz + 2 * qx*qw) * sy;
            m.m[3][1]= 0.f;
            
            // c2
            m.m[0][2]= (2 * qx*qz + 2 * qy*qw) * sz;
            m.m[1][2]= (2 * qy*qz - 2 * qx*qw) * sz;
            m.m[2][2]= (1 - 2 * qx*qx - 2 * qy*qy) * sz;
            m.m[3][2]= 0.f;
            
            // c3
            m.m[0][3]= tx;
            m.m[1][3]= ty;
            m.m[2][3]= tz;
            m.m[3][3]= 1.f;
            
            node.transform= m;
        }
        
        // etape 2: recalcul des transformations hierarchiques
        for(unsigned i= 0; i < m_nodes.size(); i++)
        {
            gltf::Node& node= m_nodes[i];
            
            Transform m= node.transform;
            int parent= node.parent_id;
            while(parent != -1)
            {
                m= m_nodes[parent].transform * m;
                parent= m_nodes[parent].parent_id;
            }
            
            node.model= m;
        }
        
        // \todo, oui c'est bourrin... utiliser un "cache" de transformations deja caclulees...
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        release_widgets(m_widgets);
        m_repere.release();
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // deplacement camera
        // recupere les mouvements de la souris
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        int mousex, mousey;
        SDL_GetMouseState(&mousex, &mousey);
        
        // deplace la camera
        if(mb & SDL_BUTTON(1))
            m_camera.rotation(mx, my);      // tourne autour de l'objet
        else if(mb & SDL_BUTTON(3))
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height()); // deplace le point de rotation
        else if(mb & SDL_BUTTON(2))
            m_camera.move(mx);           // approche / eloigne l'objet
        
        SDL_MouseWheelEvent wheel= wheel_event();
        if(wheel.y != 0)
        {
            clear_wheel_event();
            m_camera.move(8.f * wheel.y);  // approche / eloigne l'objet
        }

        const char *orbiter_filename= "app_orbiter.txt";
        // copy / export / write orbiter
        if(key_state('c'))
        {
            clear_key_state('c');
            m_camera.write_orbiter(orbiter_filename);
            
        }
        // paste / read orbiter
        if(key_state('v'))
        {
            clear_key_state('v');
            
            Orbiter tmp;
            if(tmp.read_orbiter(orbiter_filename) < 0)
                // ne pas modifer la camera en cas d'erreur de lecture...
                tmp= m_camera;
            
            m_camera= tmp;
        }

        // recharger le shader
        if(key_state('r'))
        {
            clear_key_state('r');
            
            reload_program(m_program, "tutos/gltf/viewer.glsl");
            program_print_errors(m_program);
            
            reload_program(m_skinning_program, "tutos/gltf/viewer_skinning.glsl");
            program_print_errors(m_skinning_program);
        }
        
        // . recuperer les transformations
        static int rotate= 1;
        if(key_state(' '))
        {
            clear_key_state(' ');
            rotate= (rotate + 1) % 2;
        }
        
        //~ static Transform model= Identity();
        //~ if(rotate)
            //~ model= RotationY(global_time() / 40);
        
        // dessine aussi le repere, pour le meme point de vue
        draw(m_repere, Identity(), m_camera);
        
        Transform view= m_camera.view();
        Transform projection= m_camera.projection();
        
    #if 0
        Transform mv= view * model;
        Transform mvp=projection * mv;
        
        glUseProgram(m_program);
        program_uniform(m_program, "mvpMatrix", mvp);
        program_uniform(m_program, "mvMatrix", mv);
        program_uniform(m_program, "material_color", Red());
        
        for(unsigned i= 0; i < m_meshes.size(); i++)
        {
            const gltf::Mesh& mesh= m_meshes[i];
            for(unsigned j= 0; j < mesh.primitives.size(); j++)
            {
                const gltf::Primitives& primitives= mesh.primitives[j];
                glBindVertexArray(primitives.vao);
                glDrawElements(primitives.mode, primitives.count, primitives.type, (const void *) primitives.offset);
            }
        }
    #endif
        
        // animations, interpoler les parametres des transformations des noeuds
        if(rotate)
        {
            static int animation_id= 0;
            
            // selectionner une animation, si necessaire...
            if(m_animations.size() > 1)
            {
                begin(m_widgets);
                    for(unsigned i= 0; i < m_animations.size(); i++)
                    {
                        begin_line(m_widgets);
                        select(m_widgets, m_animations[i].name.c_str(), i, animation_id);
                    }
                end(m_widgets);
                
                draw(m_widgets, window_width(), window_height());
            }
            
            play_animation(animation_id, global_time() / 1000);
            
            update_transforms();
        }
        
        {
            glUseProgram(m_program);
            program_uniform(m_program, "material_color", Color(0.8));
            program_uniform(m_program, "material_metallic", float(0));
            program_uniform(m_program, "material_roughness", float(0.5));
            
            program_use_texture(m_program, "material_color_texture", 0, m_white_texture);
            program_use_texture(m_program, "material_metallic_roughness_texture", 1, m_white_texture);
            program_use_texture(m_program, "material_occlusion_texture", 2, m_white_texture);
            
        }
        
        int draws= 0;
        
        for(unsigned i= 0; i < m_nodes.size(); i++)
        {
            const gltf::Node& node= m_nodes[i];
            if(node.mesh_id == -1)
                continue;   // rien a afficher...
            
            GLuint program= m_program;
            //~ if(node.skin_id != -1)
                //~ program= m_skinning_program;
            
            //~ glUseProgram(program);
            //~ if(node.skin_id != -1)
            //~ {
                //~ const gltf::Skeleton& skin= m_skins[node.skin_id];
                //~ std::vector<Transform> matrices(64);
                //~ for(unsigned s= 0; s < skin.nodes.size(); s++)
                    //~ matrices[s]= m_nodes[skin.nodes[s]].model * skin.matrices[s];
                
                //~ assert(program == m_skinning_program);
                //~ program_uniform(program, "matrices", matrices);
            //~ }
            
            Transform m= node.model;
            Transform mv= view * m;
            Transform mvp=projection * mv;
            
            program_uniform(program, "mvpMatrix", mvp);
            program_uniform(program, "mvMatrix", mv);
            
            const gltf::Mesh& mesh= m_meshes[node.mesh_id];
            for(unsigned k= 0; k < mesh.primitives.size(); k++)
            {
                const gltf::Primitives& primitives= mesh.primitives[k];
                
                glBindVertexArray(primitives.vao);
                
                //~ if(node.has_animation)
                    //~ // affiche les noeuds animes...
                    //~ program_uniform(program, "material_color", Color(0.8, 0.4, 0));
                //~ else
                    //~ program_uniform(program, "material_color", Color(0.8));
                
                //~ if(node.skin_id != -1)
                    //~ program_uniform(program, "material_color", Color(0.8, 0, 0));
                
                // materies
                if(primitives.material_id != -1)
                {
                    gltf::Material &material= m_materials[primitives.material_id];
                    program_uniform(program, "material_color", material.color);
                    if(material.color_texture != -1)
                        program_use_texture(program, "material_color_texture", 0, m_textures[material.color_texture]);
                    else
                        program_use_texture(program, "material_color_texture", 0, m_white_texture);
                    
                    program_uniform(program, "material_metallic", material.metallic);
                    program_uniform(program, "material_roughness", material.roughness);
                    if(material.metallic_roughness_texture != -1)
                        program_use_texture(program, "material_metallic_roughness_texture", 1, m_textures[material.metallic_roughness_texture]);
                    else
                        program_use_texture(program, "material_metallic_roughness_texture", 1, m_white_texture);
                    
                    // \todo occlusion est packee dans la meme texture que metallic_roughness (canal rouge), pas la peine d'utiliser une unite de texture en plus...
                    if(material.occlusion_texture != -1)
                        program_use_texture(program, "material_occlusion_texture", 2, m_textures[material.occlusion_texture]);
                    else
                        program_use_texture(program, "material_occlusion_texture", 2, m_white_texture);
                        
                    if(material.normal_texture != -1)
                        program_use_texture(program, "material_normal_texture", 3, m_textures[material.normal_texture]);
                    else
                        program_use_texture(program, "material_normal_texture", 3, m_white_texture);    // argh (0, 0, 1) par defaut...
                        
                    program_uniform(program, "material_emission", material.emission);
                    if(material.emission_texture != -1)
                        program_use_texture(program, "material_emission_texture", 4, m_textures[material.emission_texture]);
                    else
                        program_use_texture(program, "material_emission_texture", 4, m_black_texture);    // argh (0, 0, 0) par defaut...
                }
                
                draws++;
                if(primitives.type != GL_NONE)
                    glDrawElements(primitives.mode, primitives.count, primitives.type, (const void *) std::ptrdiff_t(primitives.offset));
                else
                    glDrawArrays(primitives.mode, 0, primitives.count);
                
                // todo : fusionner les attributs des meshs pour n'utiliser qu'un seul vao statique... au lieu d'un vao par mesh
                // et trier par shader aussi : skinning vs statique
            }
        }
        
        
        //~ printf("%d draws\n", draws);
        
        if(key_state('s'))
        {
            clear_key_state('s');
            
            static int calls= 1;
            printf("screenshot %d...\n", calls);
            screenshot("viewer", calls++);
        }
        
        // continuer...
        return 1;
    }

protected:
    Mesh m_repere;
    Orbiter m_camera;
    GLuint m_program;
    GLuint m_skinning_program;
    std::vector<GLuint> m_buffers;
    std::vector<gltf::Mesh> m_meshes;
    std::vector<gltf::Node> m_nodes;
    std::vector<gltf::Animation> m_animations;
    std::vector<gltf::Skeleton> m_skins;
    //~ std::vector<ImageData> m_images;
    std::vector<GLuint> m_textures;
    GLuint m_white_texture;
    GLuint m_black_texture;
    GLuint m_blue_texture;
    std::vector<gltf::Material> m_materials;
    
    Widgets m_widgets;
};


int main( int argc, char **argv )
{
    // il ne reste plus qu'a creer un objet application et la lancer 
    TP tp;
    tp.run();
    
    return 0;
}
