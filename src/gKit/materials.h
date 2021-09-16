
#ifndef _MATERIALS_H
#define _MATERIALS_H

#include <string>
#include <vector>
#include <cassert>

#include "color.h"


/*! representation d'une matiere d'un groupe de triangles d'un Mesh.
parametres des matieres Blinn-Phong, cf \ref matiere pour les explications et \ref brdf pour le code...
*/
struct Material
{
    Color diffuse;              //!< couleur diffuse / de base.
    Color specular;             //!< couleur du reflet.
    Color emission;             //!< pour une source de lumiere.
    float ns;                   //!< concentration des reflets, exposant pour les reflets blinn-phong.
    int diffuse_texture;        //!< indice de la texture de la couleur de base, ou -1.
    int specular_texture;        //!< indice de la texture, ou -1.
    int emission_texture;        //!< indice de la texture, ou -1.
    int ns_texture;             //!< indice de la texture de reflet, ou -1.
    
    Material( ) : diffuse(), specular(Black()), emission(), ns(0), diffuse_texture(-1), ns_texture(-1) {}
    Material( const Color& color ) : diffuse(color), specular(Black()), emission(), ns(0), diffuse_texture(-1), ns_texture(-1) {}
};


//! ensemble de matieres d'un Mesh. 
struct Materials
{
    std::vector<std::string> names;     //!< noms des matieres.
    std::vector<Material> materials;    //!< matieres.
    std::vector<std::string> texture_filenames; //!< noms des textures à charger.
    int default_material_id;    //<! inidice de la matiere par defaut dans materials.
    
    Materials( ) : names(), materials(), texture_filenames(), default_material_id(-1) {}
    
    //! ajoute une matiere.
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
    
    //! ajoute une texture / nom du fichier.
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
    
    //! recherche une matiere avec son nom. renvoie son indice dans materials, ou -1.
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
    
    //! nombre de matieres.
    int count( ) const { return int(materials.size()); }
    
    //! renvoie le nom de fichier de la ieme texture.
    const char *name( const int id ) const { assert(id != -1); assert(id < int(materials.size())); return names[id].c_str(); }
    //! renvoie le nom de fichier de la ieme texture.
    const char *name( const int id ) { assert(id != -1); assert(id < int(materials.size())); return names[id].c_str(); }
    
    //! renvoie la ieme matiere.
    const Material& material( const int id ) const { assert(id != -1); assert(id < int(materials.size())); return materials[id]; }
    //! renvoie la ieme matiere.
    Material& material( const int id ) { assert(id != -1); assert(id < int(materials.size())); return materials[id]; }
    
    //! renvoie la matiere 'name', si elle exsite.
    const Material& material( const char *name )
    {
        int id= find(name);
        assert(id != -1);
        return materials[id];
    }
    
    //! renvoie une matiere par defaut.
    const Material& default_material( )
    {
        if(default_material_id == -1)
            default_material_id= insert(Material(Color(0.8)), "default");
        
        return material(default_material_id);
    }
    
    //! indice de la matiere par defaut dans le tableau materials.
    int default_material_index( )
    {
        if(default_material_id == -1)
            default_material_id= insert(Material(Color(0.8)), "default");
        
        return default_material_id;
    }
    
    //! renvoie le nombre de noms de fichiers de textures.
    int filename_count( ) const { return int(texture_filenames.size()); }
    //! renvoie le nombre de noms de fichiers de textures.
    const char *filename( const int id ) const { return texture_filenames[id].c_str(); }
    
    //! renvoie l'indice d'une texture, si elle existe.
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
