
#ifndef _MATERIALS_H
#define _MATERIALS_H

#include <string>
#include <vector>
#include <cassert>

#include "color.h"


//! representation d'une matiere.
struct Material
{
    Color diffuse;              //!< couleur diffuse
    Color specular;             //!< couleur du reflet
    Color emission;             //!< pour une source de lumiere
    float ns;                   //!< exposant pour les reflets blinn-phong
    int diffuse_texture;        //! < indice de la texture, ou -1
    int ns_texture;             //!< indice de la texture, ou -1
    
    Material( ) : diffuse(), specular(Black()), emission(), ns(0), diffuse_texture(-1), ns_texture(-1) {}
    Material( const Color& color ) : diffuse(color), specular(Black()), emission(), ns(0), diffuse_texture(-1), ns_texture(-1) {}
};


//! ensemble de matieres.
struct Materials
{
    std::vector<std::string> names;
    std::vector<Material> materials;
    std::vector<std::string> texture_filenames;
    int default_material_id;
    
    Materials( ) : names(), materials(), texture_filenames(), default_material_id(-1) {}
    
    int insert( const Material& material, const char *name )
    {
        int id= find(name);
        if(id == -1)
        {
            id= int(materials.size());
            names.push_back(name);
            materials.push_back(material);
        }
        assert(materials.size() == names.size());
        return id;
    }
    
    int insert_texture( const char *filename )
    {
        int id= find_texture(filename);
        if(id == -1)
        {
            id= int(texture_filenames.size());
            texture_filenames.push_back(filename);
        }
        return id;
    }
    
    int find( const char *name )
    {
        if(name == nullptr || name[0] == 0)
            return -1;
        
        for(int i= 0; i < int(names.size()); i++)
        {
            if(names[i] == name)
                return i;
        }
        return -1;
    }
    
    int count( ) const { return int(materials.size()); }
    
    const char *name( const int id ) const { assert(id != -1); assert(id < int(materials.size())); return names[id].c_str(); }
    const char *name( const int id ) { assert(id != -1); assert(id < int(materials.size())); return names[id].c_str(); }
    
    const Material& material( const int id ) const { assert(id != -1); assert(id < int(materials.size())); return materials[id]; }
    Material& material( const int id ) { assert(id != -1); assert(id < int(materials.size())); return materials[id]; }
    
    const Material& material( const char *name )
    {
        int id= find(name);
        assert(id != -1);
        return materials[id];
    }
    
    const Material& default_material( )
    {
        if(default_material_id == -1)
            default_material_id= insert(Material(Color(0.8)), "default");
        
        return material(default_material_id);
    }
    
    int default_material_index( )
    {
        if(default_material_id == -1)
            default_material_id= insert(Material(Color(0.8)), "default");
        
        return default_material_id;
    }
    
    int filename_count( ) const { return int(texture_filenames.size()); }
    const char *filename( const int id ) const { return texture_filenames[id].c_str(); }
    
    int find_texture( const char *filename )
    {
        if(filename == nullptr || filename[0] == 0)
            return -1;
        
        for(int i= 0; i < int(texture_filenames.size()); i++)
        {
            if(texture_filenames[i] == filename)
                return i;
        }
        return -1;
    }
};

#endif
