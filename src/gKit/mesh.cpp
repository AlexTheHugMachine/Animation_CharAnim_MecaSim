
#include <cstdio>
#include <cassert>
#include <string>
#include <algorithm>

#include "vec.h"
#include "mesh.h"

#include "program.h"
#include "uniforms.h"

#include "window.h"


int Mesh::create( const GLenum primitives )
{
    m_primitives= primitives;
    return 0;
}

void Mesh::release( )
{
    printf("mesh release %d\n", m_vao);
    
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_buffer);
    glDeleteBuffers(1, &m_index_buffer);
}

// definit les attributs du prochain sommet
Mesh& Mesh::default_color( const Color& color )
{
    m_color= color;
    return *this;
}

Mesh& Mesh::color( const vec4& color )
{
    if(m_colors.size() <= m_positions.size())
        m_colors.push_back(color);
    else
        m_colors.back()= color;
    m_update_buffers= true;
    return *this;
}

Mesh& Mesh::normal( const vec3& normal )
{
    if(m_normals.size() <= m_positions.size())
        m_normals.push_back(normal);
    else
        m_normals.back()= normal;
    m_update_buffers= true;
    return *this;
}

Mesh& Mesh::texcoord( const vec2& uv )
{
    if(m_texcoords.size() <= m_positions.size())
        m_texcoords.push_back(uv);
    else
        m_texcoords.back()= uv;
    m_update_buffers= true;
    return *this;
}

// insere un nouveau sommet
unsigned int Mesh::vertex( const vec3& position )
{
    m_update_buffers= true;
    m_positions.push_back(position);

    // copie les autres attributs du sommet, uniquement s'ils sont definis
    if(m_texcoords.size() > 0 && m_texcoords.size() != m_positions.size())
        m_texcoords.push_back(m_texcoords.back());
    if(m_normals.size() > 0 && m_normals.size() != m_positions.size())
        m_normals.push_back(m_normals.back());
    if(m_colors.size() > 0 && m_colors.size() != m_positions.size())
        m_colors.push_back(m_colors.back());

    // copie la matiere courante, uniquement si elle est definie
    if(m_triangle_materials.size() > 0 && int(m_triangle_materials.size()) < triangle_count())
        m_triangle_materials.push_back(m_triangle_materials.back());
    
    unsigned int index= m_positions.size() -1;
    // construction de l'index buffer pour les strip
    switch(m_primitives)
    {
        case GL_LINE_STRIP:
        case GL_LINE_LOOP:
        case GL_TRIANGLE_STRIP:
        case GL_TRIANGLE_FAN:
            m_indices.push_back(index);
            break;
        default:
            break;
    }

    // renvoie l'indice du sommet
    return index;
}

// update attributes
Mesh& Mesh::color( const unsigned int id, const vec4& c )
{
    assert(id < m_colors.size());
    m_update_buffers= true;
    m_colors[id]= c;
    return *this;
}

Mesh& Mesh::normal( const unsigned int id, const vec3& n )
{
    assert(id < m_normals.size());
    m_update_buffers= true;
    m_normals[id]= n;
    return *this;
}

Mesh& Mesh::texcoord( const unsigned int id, const vec2& uv )
{
    assert(id < m_texcoords.size());
    m_update_buffers= true;
    m_texcoords[id]= uv;
    return *this;
}

void Mesh::vertex( const unsigned int id, const vec3& p )
{
    assert(id < m_positions.size());
    m_update_buffers= true;
    m_positions[id]= p;
}

void Mesh::clear( )
{
    m_update_buffers= true;
    
    m_positions.clear();
    m_texcoords.clear();
    m_normals.clear();
    m_colors.clear();
    m_indices.clear();
    //~ m_materials.clear();
    m_triangle_materials.clear();
}

//
Mesh& Mesh::triangle( const unsigned int a, const unsigned int b, const unsigned int c )
{
    assert(a < m_positions.size());
    assert(b < m_positions.size());
    assert(c < m_positions.size());
    m_update_buffers= true;
    m_indices.push_back(a);
    m_indices.push_back(b);
    m_indices.push_back(c);
    
    // copie la matiere courante, uniquement si elle est definie
    if(m_triangle_materials.size() > 0 && int(m_triangle_materials.size()) < triangle_count())
        m_triangle_materials.push_back(m_triangle_materials.back());
    
    m_update_buffers= true;
    return *this;
}

Mesh& Mesh::triangle_last( const int a, const int b, const int c )
{
    assert(a < 0);
    assert(b < 0);
    assert(c < 0);
    m_update_buffers= true;
    m_indices.push_back(int(m_positions.size()) + a);
    m_indices.push_back(int(m_positions.size()) + b);
    m_indices.push_back(int(m_positions.size()) + c);
    
    // copie la matiere courante, uniquement si elle est definie
    if(m_triangle_materials.size() > 0 && int(m_triangle_materials.size()) < triangle_count())
        m_triangle_materials.push_back(m_triangle_materials.back());
    
    m_update_buffers= true;
    return *this;
}

Mesh& Mesh::restart_strip( )
{
    m_update_buffers= true;
    m_indices.push_back(~0u);   // ~0u plus grand entier non signe representable, ou UINT_MAX...
#if 1
    glPrimitiveRestartIndex(~0u);
    glEnable(GL_PRIMITIVE_RESTART);
#else
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX); // n'existe pas sur mac ?!
#endif
    return *this;
}

Mesh& Mesh::index( const int a )
{
    if(a < 0)
        m_indices.push_back(int(m_positions.size()) + a);
    else if(a < int(m_positions.size()))
        m_indices.push_back(a);
    else
    {
        printf("[error] Mesh::index(): invalid index...\n");
        return *this;   // erreur
    }
    
    // copie la matiere courante, uniquement si elle est definie
    if(m_triangle_materials.size() > 0 && int(m_triangle_materials.size()) < triangle_count())
        m_triangle_materials.push_back(m_triangle_materials.back());
    
    m_update_buffers= true;
    return *this;
}


Materials& Mesh::materials( )
{
    return m_materials;
}

const Materials& Mesh::materials( ) const
{
    return m_materials;
}

void Mesh::materials( const Materials& materials )
{
    m_materials= materials;
}

Mesh& Mesh::material( const unsigned int id )
{
    if(int(m_triangle_materials.size()) <= triangle_count())
        m_triangle_materials.push_back(id);
    else
        m_triangle_materials.back()= id;
    m_update_buffers= true;
    return *this;
}

const std::vector<unsigned int>& Mesh::material_indices( ) const
{
    return m_triangle_materials;
}

int Mesh::triangle_material_index( const unsigned int id ) const
{
    assert((size_t) id < m_triangle_materials.size());
    return m_triangle_materials[id];
}

const Material &Mesh::triangle_material( const unsigned int id ) const
{
    assert((size_t) id < m_triangle_materials.size());
    return m_materials.material(m_triangle_materials[id]);
}


std::vector<TriangleGroup> Mesh::groups( )
{
    return groups(m_triangle_materials);
}

std::vector<TriangleGroup> Mesh::groups( const std::vector<unsigned int>& triangle_properties )
{
    if(m_primitives != GL_TRIANGLES)
        return {};
    
    // pas le bon nombre d'infos, renvoyer un seul groupe
    if(int(triangle_properties.size()) != triangle_count())
    {
        if(m_indices.size())
            return { {0, 0, int(m_indices.size())} };
        else
            return { {0, 0, int(m_positions.size())} };
    }
    
    // trie les triangles
    std::vector<int> remap(triangle_count());
    for(unsigned i= 0; i < remap.size(); i++)
        remap[i]= i;

    struct triangle_sort
    {
        const std::vector<unsigned int>& properties;
        
        triangle_sort( const std::vector<unsigned int>& _properties ) : properties(_properties) {}
        
        bool operator() ( const int& a, const int& b ) const
        {
            return properties[a] < properties[b];
        }
    };
    
    std::stable_sort(remap.begin(), remap.end(), triangle_sort(triangle_properties));
    
    // re-organise les triangles, et construit les groupes
    std::vector<TriangleGroup> groups;
    if(m_indices.size())
    {
        int first= 0;
        int property_id= triangle_properties[remap[0]];
        
        // re-organise l'index buffer...
        std::vector<unsigned int> indices;
        std::vector<unsigned int> material_indices;
        for(unsigned i= 0; i < remap.size(); i++)
        {
            int id= triangle_properties[remap[i]];
            if(id != property_id)
            {
                groups.push_back( {property_id, first, int(3*i) - first} );
                first= 3*i;
                property_id= id;
            }
            
            indices.push_back(m_indices[3*remap[i]]);
            indices.push_back(m_indices[3*remap[i]+1]);
            indices.push_back(m_indices[3*remap[i]+2]);
            
            material_indices.push_back(m_triangle_materials[remap[i]]);
        }
        
        // dernier groupe
        groups.push_back( {property_id, first, int(3 * remap.size()) - first} );
        
        std::swap(m_indices, indices);
        std::swap(m_triangle_materials, material_indices);
    }
    else
    {
        int first= 0;
        int property_id= triangle_properties[remap[0]];
        
        // re-organise les attributs !!
        std::vector<vec3> positions;
        std::vector<vec2> texcoords;
        std::vector<vec3> normals;
        std::vector<vec4> colors;
        std::vector<unsigned int> material_indices;
        for(unsigned i= 0; i < remap.size(); i++)
        {
            int id= triangle_properties[remap[i]];
            if(id != property_id)
            {
                groups.push_back( {property_id, first, int(3*i) - first} );
                first= 3*i;
                property_id= id;
            }
            
            positions.push_back(m_positions[3*remap[i]]);
            positions.push_back(m_positions[3*remap[i]+1]);
            positions.push_back(m_positions[3*remap[i]+2]);
            if(has_texcoord())
            {
                texcoords.push_back(m_texcoords[3*remap[i]]);
                texcoords.push_back(m_texcoords[3*remap[i]+1]);
                texcoords.push_back(m_texcoords[3*remap[i]+2]);
            }
            if(has_normal())
            {
                normals.push_back(m_normals[3*remap[i]]);
                normals.push_back(m_normals[3*remap[i]+1]);
                normals.push_back(m_normals[3*remap[i]+2]);
            }
            if(has_color())
            {
                colors.push_back(m_colors[3*remap[i]]);
                colors.push_back(m_colors[3*remap[i]+1]);
                colors.push_back(m_colors[3*remap[i]+2]);
            }
            
            material_indices.push_back(m_triangle_materials[remap[i]]);
        }
        
        // dernier groupe
        groups.push_back( {property_id, first, int(3 * remap.size()) - first} );
        
        std::swap(m_positions, positions);
        std::swap(m_texcoords, texcoords);
        std::swap(m_normals, normals);
        std::swap(m_colors, colors);
        std::swap(m_triangle_materials, material_indices);
    }
    
    return groups;
}

int Mesh::triangle_count( ) const
{
    if(m_primitives != GL_TRIANGLES)
        return 0;
    
    if(m_indices.size() > 0)
        return int(m_indices.size() / 3);
    else
        return int(m_positions.size() / 3);
}

TriangleData Mesh::triangle( const unsigned int id ) const
{
    unsigned int a, b, c;
    if(m_indices.size() > 0)
    {
        assert((size_t) id*3+2 < m_indices.size());
        a= m_indices[id*3];
        b= m_indices[id*3 +1];
        c= m_indices[id*3 +2];
    }
    else
    {
        assert((size_t) id*3+2 < m_positions.size());
        a= id*3;
        b= id*3 +1;
        c= id*3 +2;
    }
    
    TriangleData triangle;
    triangle.a= m_positions[a];
    triangle.b= m_positions[b];
    triangle.c= m_positions[c];
    
    if(m_normals.size() == m_positions.size())
    {
        triangle.na= m_normals[a];
        triangle.nb= m_normals[b];
        triangle.nc= m_normals[c];
    }
    else
    {
        // calculer la normale geometrique
        Vector ab= Point(m_positions[b]) - Point(m_positions[a]);
        Vector ac= Point(m_positions[c]) - Point(m_positions[a]);
        Vector n= normalize(cross(ab, ac));
        triangle.na= vec3(n);
        triangle.nb= vec3(n);
        triangle.nc= vec3(n);
    }
    
    if(m_texcoords.size() == m_positions.size())
    {
        triangle.ta= m_texcoords[a];
        triangle.tb= m_texcoords[b];
        triangle.tc= m_texcoords[c];
    }
    else
    {
        // coordonnees barycentriques des sommets, convention p(u, v)= w*a + u*b + v*c, avec w= 1 - u -v
        triangle.ta= vec2(0, 0);        // w= 1
        triangle.tb= vec2(1, 0);        // w= 0
        triangle.tc= vec2(0, 1);        // w= 0
    }
    
    return triangle;
}

void Mesh::bounds( Point& pmin, Point& pmax ) const
{
    if(m_positions.size() < 1)
        return;

    pmin= Point(m_positions[0]);
    pmax= pmin;

    for(unsigned i= 1; i < m_positions.size(); i++)
    {
        vec3 p= m_positions[i];
        pmin= Point( std::min(pmin.x, p.x), std::min(pmin.y, p.y), std::min(pmin.z, p.z) );
        pmax= Point( std::max(pmax.x, p.x), std::max(pmax.y, p.y), std::max(pmax.z, p.z) );
    }
}





//! buffer unique de copie / mise a jour des vertex buffers statiques. singleton. tous les meshs utilisent le meme buffer de copie...
class UpdateBuffer
{
public:
    //! transfere les donnees dans un buffer statique.
    void copy( GLenum target, const size_t offset, const size_t length, const void *data )
    {
        if(m_buffer == 0)
            glGenBuffers(1, &m_buffer);
        
        assert(m_buffer);
        glBindBuffer(GL_COPY_READ_BUFFER, m_buffer);
        if(length > m_size)
        {
            m_size= (length / (16*1024*1024) + 1) * (16*1024*1024); // alloue par bloc de 16Mo 
            assert(m_size >= length);
            
            // alloue un buffer intermediaire dynamique...
            glBufferData(GL_COPY_READ_BUFFER, m_size, nullptr, GL_DYNAMIC_DRAW);
            printf("[UpdateBuffer] allocate %dMo staging buffer...\n", int(m_size / 1024 / 1024));
        }
        
        // place les donnees dans le buffer intermediaire
        glBufferSubData(GL_COPY_READ_BUFFER, 0, length, data);
        // copie les donnees dans le vertex buffer statique
        glCopyBufferSubData(GL_COPY_READ_BUFFER, target, 0, offset, length);
    }
    
    //! detruit le buffer.
    ~UpdateBuffer( ) 
    { 
        release(); 
    }
    
    //! detruit le buffer.
    void release( )
    {
        glDeleteBuffers(1, &m_buffer);
        m_buffer= 0;
        m_size= 0;
    }
    
    //! acces au singleton.
    static UpdateBuffer& manager( )
    {
        static UpdateBuffer buffer;
        return buffer;
    }
    
protected:
    //! constructeur prive. singleton.
    UpdateBuffer( ) : m_buffer(0), m_size(0) {}
    
    GLuint m_buffer;
    size_t m_size;
};


GLuint Mesh::create_buffers( const bool use_texcoord, const bool use_normal, const bool use_color, const bool use_material_index )
{
    if(m_positions.size() == 0)
        return 0;

#if 1
    if(use_texcoord && !has_texcoord())
        printf("[oops] mesh: no texcoord array...\n");
    if(use_normal && !has_normal())
        printf("[oops] mesh: no normal array...\n");
    if(use_color && !has_color())
        printf("[oops] mesh: no color array...\n");
    if(use_material_index && !has_material_index())
        printf("[oops] mesh: no material index array...\n");
#endif
    
#ifndef __EMSCRIPTEN__
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
#else
    m_vao = 1 ;
#endif
    
    // determine la taille du buffer pour stocker tous les attributs et les indices
    m_vertex_buffer_size= vertex_buffer_size();
    if(use_texcoord && has_texcoord())
        m_vertex_buffer_size+= texcoord_buffer_size();
    if(use_normal && has_normal())
        m_vertex_buffer_size+= normal_buffer_size();
    if(use_color && has_color())
        m_vertex_buffer_size+= color_buffer_size();
    if(use_material_index && has_material_index())
        m_vertex_buffer_size+= m_positions.size() * sizeof(unsigned char);
    
    // alloue le buffer
    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertex_buffer_size, nullptr, GL_STATIC_DRAW);
    
    // transferer les attributs et configurer le format de sommet (vao)
    GLint location = 0 ;
    size_t offset= 0;
    size= vertex_buffer_size();
#ifdef __EMSCRIPTEN__
    location = glGetAttribLocation(m_program, "position") ;
    if(location != -1) {
#endif
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, vertex_buffer());
    glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, (const void *) offset);
    glEnableVertexAttribArray(location);
#ifdef __EMSCRIPTEN__
    }
#endif
    
    if(m_texcoords.size() == m_positions.size() && use_texcoord)
    {
        location = 1 ;
#ifdef __EMSCRIPTEN__
        location = glGetAttribLocation(m_program, "texcoord") ;
        if(location != -1) {
#endif
        offset= offset + size;
        size= texcoord_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, texcoord_buffer());
        glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 0, (const void *) offset);
        glEnableVertexAttribArray(location);
#ifdef __EMSCRIPTEN__
        }
#endif
    }
    
    if(m_normals.size() == m_positions.size() && use_normal)
    {
        location = 2 ;
#ifdef __EMSCRIPTEN__
        location = glGetAttribLocation(m_program, "normal") ;
        if(location != -1) {
#endif
        offset= offset + size;
        size= normal_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, normal_buffer());
        glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, (const void *) offset);
        glEnableVertexAttribArray(location);
#ifdef __EMSCRIPTEN__
        }
#endif
    }
    
    if(m_colors.size() == m_positions.size() && use_color)
    {
        location = 3 ;
#ifdef __EMSCRIPTEN__
        location = glGetAttribLocation(m_program, "color") ;
        if(location != -1) {
#endif
        offset= offset + size;
        size= color_buffer_size();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, color_buffer());
        glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, 0, (const void *) offset);
        glEnableVertexAttribArray(location);
#ifdef __EMSCRIPTEN__
        }
#endif
    }
    
    // allouer l'index buffer
    if(index_buffer_size())
    {
        glGenBuffers(1, &m_index_buffer);    
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_size, index_buffer(), GL_STATIC_DRAW);
    }

    // transfere les donnees dans les buffers
    update_buffers(use_texcoord,  use_normal, use_color, use_material_index);
    
    return m_vao;
}

int Mesh::update_buffers( const bool use_texcoord, const bool use_normal, const bool use_color, const bool use_material_index )
{
    assert(m_vao > 0);
    assert(m_buffer > 0);
    if(!m_update_buffers)
        return 0;

    // alloue un buffer de copie, necessaire pour transferer plus de 256Mo... cf tuto_stream.cpp / transfert de donnees gpu
    UpdateBuffer& update= UpdateBuffer::manager();
    
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    
    // determine la taille du buffer pour stocker tous les attributs et les indices
    size_t size= vertex_buffer_size();
    if(use_texcoord && has_texcoord())
        size+= texcoord_buffer_size();
    if(use_normal && has_normal())
        size+= normal_buffer_size();
    if(use_color && has_color())
        size+= color_buffer_size();
    if(use_material_index && has_material_index())
        size+= m_positions.size() * sizeof(unsigned char);
    
    if(size != m_vertex_buffer_size)
    {
        m_vertex_buffer_size= size;
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
    }
    
    // transferer les attributs et configurer le format de sommet (vao)
    size_t offset= 0;
    size= vertex_buffer_size();
    //~ glBufferSubData(GL_ARRAY_BUFFER, offset, size, vertex_buffer());        // copie les donnees dans le vertex buffer
    update.copy(GL_ARRAY_BUFFER, offset, size, vertex_buffer());                // copie les donnees dans le vertex buffer
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *) offset);
    glEnableVertexAttribArray(0);
    
    if(use_texcoord && has_texcoord())
    {
        offset= offset + size;
        size= texcoord_buffer_size();
        //~ glBufferSubData(GL_ARRAY_BUFFER, offset, size, texcoord_buffer());  // copie les donnees dans le vertex buffer
        update.copy(GL_ARRAY_BUFFER, offset, size, texcoord_buffer());          // copie les donnees dans le vertex buffer
        
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const void *) offset);
        glEnableVertexAttribArray(1);
    }
    
    if(use_normal && has_normal())
    {
        offset= offset + size;
        size= normal_buffer_size();
        //~ glBufferSubData(GL_ARRAY_BUFFER, offset, size, normal_buffer());    // copie les donnees dans le vertex buffer
        update.copy(GL_ARRAY_BUFFER, offset, size, normal_buffer());            // copie les donnees dans le vertex buffer
        
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const void *) offset);
        glEnableVertexAttribArray(2);
    }
    
    if(use_color && has_color())
    {
        offset= offset + size;
        size= color_buffer_size();
        //~ glBufferSubData(GL_ARRAY_BUFFER, offset, size, color_buffer());     // copie les donnees dans le vertex buffer
        update.copy(GL_ARRAY_BUFFER, offset, size, color_buffer());             // copie les donnees dans le vertex buffer
        
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (const void *) offset);
        glEnableVertexAttribArray(3);
    }
    
    if(use_material_index && has_material_index())
    {
        assert(int(m_triangle_materials.size()) == triangle_count());
        
        offset= offset + size;
        size= m_positions.size() * sizeof(unsigned char);
        
        // prepare un indice de matiere par sommet / 3 indices par triangle
        std::vector<unsigned char> buffer(m_positions.size());
        if(m_indices.size())
        {
            //!! ne fonctionne que parce que read_indexed_mesh() duplique les sommets partages par 2 matieres !!
            //!! ca ne fonctionnera probablement pas avec les mesh indexes construits par l'application... mais c'est long de detecter le probleme...
            for(int triangle_id= 0; triangle_id < int(m_triangle_materials.size()); triangle_id++)
            {
                int material_id= m_triangle_materials[triangle_id];
                assert(triangle_id*3+2 < int(m_indices.size()));
                unsigned a= m_indices[triangle_id*3];
                unsigned b= m_indices[triangle_id*3 +1];
                unsigned c= m_indices[triangle_id*3 +2];
                
                buffer[a]= material_id;
                buffer[b]= material_id;
                buffer[c]= material_id;
            }
        }
        else
        {
            for(int triangle_id= 0; triangle_id < int(m_triangle_materials.size()); triangle_id++)
            {
                int material_id= m_triangle_materials[triangle_id];
                assert(triangle_id*3+2 < int(m_positions.size()));
                unsigned a= triangle_id*3;
                unsigned b= triangle_id*3 +1;
                unsigned c= triangle_id*3 +2;
                
                buffer[a]= material_id;
                buffer[b]= material_id;
                buffer[c]= material_id;
            }
        }
        
        //~ glBufferSubData(GL_ARRAY_BUFFER, offset, size, buffer.data());      // copie les donnees dans le vertex buffer
        update.copy(GL_ARRAY_BUFFER, offset, size, buffer.data());              // copie les donnees dans le vertex buffer
        
        glVertexAttribIPointer(4, 1, GL_UNSIGNED_BYTE, 0, (const void *) offset);
        glEnableVertexAttribArray(4);
    }
    
    // index buffer
    size= index_buffer_size();
    if(size != m_index_buffer_size)
    {
        m_index_buffer_size= index_buffer_size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size(), index_buffer(), GL_STATIC_DRAW);
    }
    
    m_update_buffers= false;
    return 1;
}

void Mesh::draw( const GLuint program, const bool use_position, const bool use_texcoord, const bool use_normal, const bool use_color, const bool use_material_index )
{
    std::string definitions;

    if(m_texcoords.size() == m_positions.size() && use_texcoord)
        definitions.append("#define USE_TEXCOORD\n");
    if(m_normals.size() == m_positions.size() && use_normal)
        definitions.append("#define USE_NORMAL\n");
    if(m_colors.size() == m_positions.size() && use_color)
        definitions.append("#define USE_COLOR\n");
    if(use_light)
        definitions.append("#define USE_LIGHT\n");
    if(use_texcoord && use_alpha_test)
        definitions.append("#define USE_ALPHATEST\n");

    //~ printf("--\n%s", definitions.c_str());
    bool use_mesh_color= (m_primitives == GL_POINTS || m_primitives == GL_LINES || m_primitives == GL_LINE_STRIP || m_primitives == GL_LINE_LOOP);
#ifndef __EMSCRIPTEN__
    if(!use_mesh_color)
        m_program= read_program( smart_path("data/shaders/mesh.glsl"), definitions.c_str());
    else
        m_program= read_program( smart_path("data/shaders/mesh_color.glsl"), definitions.c_str());
#else
    if(!use_mesh_color)
        m_program= read_program( smart_path("data/shaders/es2_mesh.glsl"), definitions.c_str());
    else
        m_program= read_program( smart_path("data/shaders/es2_mesh_color.glsl"), definitions.c_str());
#endif
    return m_program;
}


void Mesh::draw( const Transform& model, const Transform& view, const Transform& projection,
    const bool use_light, const Point& light, const Color& light_color,
    const bool use_texture, const GLuint texture,
    const bool use_alpha_test, const float alpha_min )
{
    bool use_texcoord= (m_texcoords.size() == m_positions.size() && texture > 0);
    bool use_normal= (m_normals.size() == m_positions.size());
    bool use_color= (m_colors.size() == m_positions.size());
    
    // etape 1 : construit le program en fonction des attributs du mesh et des options choisies
    unsigned int key= 0;
    if(use_texcoord) key= key | 1;
    if(use_normal) key= key | 2;
    if(use_color) key= key | 4;
    if(use_texture) key= key | 8;
    if(use_light) key= key | 16;
    if(use_alpha_test) key= key | 32;

    if(m_state != key)
        // recherche un shader deja compile pour ce type de draw
        m_program= m_state_map[key];

    if(m_program == 0)
    {
        // pas de shader pour ce type de draw
        create_program(use_texcoord, use_normal, use_color, use_light, use_alpha_test);
        program_print_errors(m_program);

        // conserver le shader
        m_state_map[key]= m_program;
    }

    // conserve la config du shader selectionne.
    assert(m_program != 0);
    m_state= key;

    glUseProgram(m_program);
    program_uniform(m_program, "mesh_color", default_color());

    Transform mv= view * model;
    Transform mvp= projection * mv;

    program_uniform(m_program, "mvpMatrix", mvp);
    program_uniform(m_program, "mvMatrix", mv);
    if(use_normal)
        program_uniform(m_program, "normalMatrix", mv.normal()); // transforme les normales dans le repere camera.

    // utiliser une texture, elle ne sera visible que si le mesh a des texcoords...
    if(texture && use_texcoord && use_texture)
        program_use_texture(m_program, "diffuse_color", 0, texture);

    if(use_light)
    {
        program_uniform(m_program, "light", view(light));       // transforme la position de la source dans le repere camera, comme les normales
        program_uniform(m_program, "light_color", light_color);
    }
    
    if(use_alpha_test)
        program_uniform(m_program, "alpha_min", alpha_min);
    
    // etape  2 : cree les buffers et le vao
    if(m_vao == 0)
        create_buffers(true, true, true);
#ifndef __EMSCRIPTEN__
    assert(m_vao != 0);
#endif
    
    if(m_update_buffers)
        update_buffers(true, true, true);
    
    glBindVertexArray(m_vao);
    
    // etape 3 : dessiner
    if(m_indices.size() > 0)
        glDrawElements(m_primitives, (GLsizei) m_indices.size(), GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(m_primitives, 0, (GLsizei) m_positions.size());
}

void Mesh::draw( const GLuint program, const bool use_position, const bool use_texcoord, const bool use_normal, const bool use_color )
{
    if(program == 0)
    {
        printf("[oops]  no program... can't draw !!\n");
        return;
    }
    
    // transfere toutes les donnees disponibles (et correctement definies)
    // le meme mesh peut etre dessine avec plusieurs shaders utilisant des attributs differents... 
    if(m_vao == 0)
        // cree les buffers demandes, inclus toujours position
        create_buffers(use_texcoord, use_normal, use_color);
#ifndef __EMSCRIPTEN__
    assert(m_vao != 0);
#endif
    
    if(m_update_buffers)
        update_buffers(use_texcoord, use_normal, use_color);
#ifndef __EMSCRIPTEN__
    glBindVertexArray(m_vao);
#endif
    
    #ifndef GK_RELEASE
    {
        char label[2048]= { 0 };
        #ifdef GL_VERSION_4_3
        {
            char tmp[1024];
            glGetObjectLabel(GL_PROGRAM, program, sizeof(tmp), nullptr, tmp);
            sprintf(label, "program( %u '%s' )", program, tmp);
        }
        #else
            sprintf(label, "program( %u )", program); 
        #endif
        
        // verifie que le program est selectionne
        GLuint current;
        glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *) &current);
        if(current != program)
            printf("[oops] %s: not active... undefined draw !!\n", label); 
        
        // verifie que les attributs necessaires a l'execution du shader sont presents dans le mesh...
        // etape 1 : recuperer le nombre d'attributs
        GLint n= 0;
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &n);
        
        // etape 2 : recuperer les infos de chaque attribut
        char name[1024];
        for(int index= 0; index < n; index++)
        {
            GLint glsl_size;
            GLenum glsl_type;
            glGetActiveAttrib(program, index, sizeof(name), nullptr, &glsl_size, &glsl_type, name);
            
            GLint location= glGetAttribLocation(program, name);
            if(location == 0)       // attribut position necessaire a l'execution du shader
            {
                if(!use_position || !has_position())
                    printf("[oops] position attribute '%s' in %s: no data... undefined draw !!\n", name, label);
                if(glsl_size != 1 || glsl_type != GL_FLOAT_VEC3)
                    printf("[oops] position attribute '%s' is not declared as a vec3 in %s... undefined draw !!\n", name, label);
            }
            else if(location == 1)  // attribut texcoord necessaire 
            {
                if(!use_texcoord || !has_texcoord())
                    printf("[oops] texcoord attribute '%s' in %s: no data... undefined draw !!\n", name, label);
                if(glsl_size != 1 || glsl_type != GL_FLOAT_VEC2)
                    printf("[oops] texcoord attribute '%s' is not declared as a vec2 in %s... undefined draw !!\n", name, label);
            }
            else if(location == 2)  // attribut normal necessaire
            {
                if(!use_normal || !has_normal())
                    printf("[oops] normal attribute '%s' in %s: no data... undefined draw !!\n", name, label);
                if(glsl_size != 1 || glsl_type != GL_FLOAT_VEC3)
                    printf("[oops] attribute '%s' is not declared as a vec3 in %s... undefined draw !!\n", name, label);
            }
            else if(location == 3)  // attribut color necessaire
            {
                if(!use_color || !has_color())
                    printf("[oops] color attribute '%s' in %s: no data... undefined draw !!\n", name, label);
                if(glsl_size != 1 || glsl_type != GL_FLOAT_VEC4)
                    printf("[oops] attribute '%s' is not declared as a vec4 in %s... undefined draw !!\n", name, label);
            }
            else if(location == 4)  // attribut material_index necessaire
            {
                if(!use_material_index || !has_material_index())
                    printf("[oops] material_index attribute '%s' in %s: no data... undefined draw !!\n", name, label);
                if(glsl_size != 1 || glsl_type != GL_UNSIGNED_INT)
                    printf("[oops] attribute '%s' is not declared as a uint in %s... undefined draw !!\n", name, label);
            }
        }
    }
    #endif
    
    if(m_indices.size() > 0)
        glDrawElements(m_primitives, n, GL_UNSIGNED_INT, (void *) (first * sizeof(unsigned)));
    else
        glDrawArrays(m_primitives, first, n);
}
