
#include <cstdio>
#include <cassert>
#include <string>
#include <algorithm>

#include "program.h"
#include "uniforms.h"
#include "buffer.h"
#include "mesh.h"


int Mesh::create( const GLenum primitives )
{
    m_primitives= primitives; 
    return 0; 
}

void Mesh::release( )
{
    if(m_vao)
        release_vertex_format(m_vao);
    if(m_program)
        release_program(m_program);
}

Mesh& Mesh::default_color( const Color& color ) 
{
    m_color= color; 
    return *this; 
}

Mesh& Mesh::color( const vec4& color )
{
    m_colors.push_back(color);
    return *this;
}

Mesh& Mesh::normal( const vec3& normal )
{
    m_normals.push_back(normal);
    return *this;
}

Mesh& Mesh::texcoord( const vec2& uv )
{
    m_texcoords.push_back(uv);
    return *this;
}

unsigned int Mesh::vertex( const vec3& position )
{
    m_positions.push_back(position);

    // copie les autres attributs du sommet, uniquement s'ils sont definis
    if(m_texcoords.size() > 0 && m_texcoords.size() != m_positions.size())
        m_texcoords.push_back(m_texcoords.back());
    if(m_normals.size() > 0 && m_normals.size() != m_positions.size())
        m_normals.push_back(m_normals.back());
    if(m_colors.size() > 0 && m_colors.size() != m_positions.size())
        m_colors.push_back(m_colors.back());
    
    unsigned int index= (unsigned int) m_positions.size() -1;
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

Mesh& Mesh::update_color( const unsigned int id, const vec4& color )
{
    assert(id < m_colors.size());
    m_update_buffers= true;
    m_colors[id]= color;
    return *this;
}

Mesh& Mesh::update_normal( const unsigned int id, const vec3& normal )
{
    assert(id < m_normals.size());
    m_update_buffers= true;
    m_normals[id]= normal;
    return *this;
}

Mesh& Mesh::update_texcoord( const unsigned int id, const vec2& uv )
{
    assert(id < m_texcoords.size());
    m_update_buffers= true;
    m_texcoords[id]= uv;
    return *this;
}

void Mesh::update_vertex( const unsigned int id, const vec3& position )
{
    assert(id < m_positions.size());
    m_update_buffers= true;
    m_positions[id]= position;
}

Mesh& Mesh::triangle( const unsigned int a, const unsigned int b, const unsigned int c )
{
    assert(a < m_positions.size());
    assert(b < m_positions.size());
    assert(c < m_positions.size());
    m_indices.push_back(a);
    m_indices.push_back(b);
    m_indices.push_back(c);
    return *this;
}

Mesh& Mesh::triangle_last( const int a, const int b, const int c )
{
    assert(a < 0);
    assert(b < 0);
    assert(c < 0);
    m_indices.push_back((int) m_positions.size() + a);
    m_indices.push_back((int) m_positions.size() + b);
    m_indices.push_back((int) m_positions.size() + c);
    return *this;
}

Mesh& Mesh::restart_strip( )
{
    m_indices.push_back(~0u);   // ~0u plus grand entier non signe representable
#if 1
    glPrimitiveRestartIndex(~0u);
    glEnable(GL_PRIMITIVE_RESTART);
#else
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX); // n'existe pas sur mac ?!
#endif
    return *this;
}

void Mesh::bounds( Point& pmin, Point& pmax )
{
    if(m_positions.size() < 1)
        return;
    
    pmin= Point(m_positions[0]);
    pmax= pmin; 
    
    for(unsigned int i= 1; i < (unsigned int) m_positions.size(); i++)
    {
        vec3 p= m_positions[i];
        pmin= Point( std::min(pmin.x, p.x), std::min(pmin.y, p.y), std::min(pmin.z, p.z) );
        pmax= Point( std::max(pmax.x, p.x), std::max(pmax.y, p.y), std::max(pmax.z, p.z) );
    }
}

const void *Mesh::attribute_buffer( const unsigned int id ) const 
{
    assert(id < 4);
    switch(id)
    {
        case 0: return vertex_buffer(); break;
        case 1: return texcoord_buffer(); break;
        case 2: return normal_buffer(); break;
        case 3: return color_buffer(); break;
        default: return nullptr;
    }
}

std::size_t Mesh::attribute_buffer_size( const unsigned int id ) const 
{
    assert(id < 4);
    switch(id)
    {
        case 0: return vertex_buffer_size(); break;
        case 1: return texcoord_buffer_size(); break;
        case 2: return normal_buffer_size(); break;
        case 3: return color_buffer_size(); break;
        default: return 0;
    }
}


GLuint Mesh::create_buffers( const bool use_texcoord, const bool use_normal, const bool use_color )
{
    if(m_positions.size() == 0)
        return 0;
    
    // ne creer que les buffers necessaires
    GLuint vao= create_vertex_format();
    make_vertex_buffer(vao, 0,  3, GL_FLOAT, vertex_buffer_size(), vertex_buffer());

    if(m_indices.size() > 0)
        make_index_buffer(vao, index_buffer_size(), index_buffer());

#if 1
    if(m_texcoords.size() > 0 && m_texcoords.size() < m_positions.size() && use_texcoord)
        printf("[error] invalid texcoords array...\n");
    if(m_normals.size() > 0 && m_normals.size() < m_positions.size() && use_normal)
        printf("[error] invalid normals array...\n");
    if(m_colors.size() > 0 && m_colors.size() < m_positions.size() && use_color)
        printf("[error] invalid colors array...\n");
#endif

    if(m_texcoords.size() == m_positions.size() && use_texcoord)
        make_vertex_buffer(vao, 1,  2, GL_FLOAT, texcoord_buffer_size(), texcoord_buffer());
    if(m_normals.size() == m_positions.size() && use_normal)
        make_vertex_buffer(vao, 2,  3, GL_FLOAT, normal_buffer_size(), normal_buffer());
    if(m_colors.size() == m_positions.size() && use_color)
        make_vertex_buffer(vao, 3,  4, GL_FLOAT, color_buffer_size(), color_buffer());
    
    m_update_buffers= false;
    return vao;
}

int Mesh::update_buffers( const bool use_texcoord, const bool use_normal, const bool use_color )
{
    assert(m_vao > 0);
    if(!m_update_buffers)
        return 0;
    
    glBindVertexArray(m_vao);
    update_vertex_buffer(m_vao, 0, vertex_buffer_size(), vertex_buffer());
    
    // ne modifier que les attributs des sommets, pas la topologie / structure du maillage
    if(m_texcoords.size() == m_positions.size() && use_texcoord)
        update_vertex_buffer(m_vao, 1, texcoord_buffer_size(), texcoord_buffer());
    if(m_normals.size() == m_positions.size() && use_normal)
        update_vertex_buffer(m_vao, 2, normal_buffer_size(), normal_buffer());
    if(m_colors.size() == m_positions.size() && use_color)
        update_vertex_buffer(m_vao, 3, color_buffer_size(), color_buffer());
    
    m_update_buffers= false;
    return 1;
}


GLuint Mesh::create_program( const bool use_texcoord, const bool use_normal, const bool use_color )
{
    std::string definitions;

    if(m_texcoords.size() > 0 && use_texcoord)
        definitions.append("#define USE_TEXCOORD\n");
    if(m_normals.size() > 0 && use_normal)
        definitions.append("#define USE_NORMAL\n");
    if(m_colors.size() > 0 && use_color)
        definitions.append("#define USE_COLOR\n");
    
    bool use_mesh_color= (m_primitives == GL_POINTS || m_primitives == GL_LINES || m_primitives == GL_LINE_STRIP || m_primitives == GL_LINE_LOOP);
    if(!use_mesh_color)
    {
        //~ if(definitions.size() > 0) printf("[mesh program definitions]\n%s", definitions.c_str());
        return read_program("data/shaders/mesh.glsl", definitions.c_str());
    }
    else
    {
        //~ if(definitions.size() > 0) printf("[mesh color program definitions]\n%s", definitions.c_str());
        return read_program("data/shaders/mesh_color.glsl", definitions.c_str());
    }
}


void Mesh::draw( const Transform& model, const Transform& view, const Transform& projection, const GLuint texture )
{
    bool use_texcoord= (m_texcoords.size() == m_positions.size() && texture > 0);
    bool use_normal= (m_normals.size() == m_positions.size());
    bool use_color= (m_colors.size() == m_positions.size());
    
    if(m_vao == 0)
        m_vao= create_buffers(use_texcoord, use_normal, use_color);
    if(m_update_buffers)
        update_buffers(use_texcoord, use_normal, use_color);
    
    if(m_program == 0)
    {
    #if 1
        if(m_texcoords.size() > 0 && texture == 0)
            printf("[mesh draw] texcoords, no texture... use_texcoord= %s\n", use_texcoord ? "true" : "false");
        if(m_texcoords.size() == 0 && texture > 0)
            printf("[mesh draw] no texcoords, texture... use_texcoord= %s\n", use_texcoord ? "true" : "false");
    #endif
        
        m_program= create_program(use_texcoord, use_normal, use_color);
        program_print_errors(m_program);
    }
    
    glBindVertexArray(m_vao);
    glUseProgram(m_program);
    
    program_uniform(m_program, "mesh_color", default_color());
    
    Transform mv= view * model;
    Transform normal= make_normal_transform(mv);
    Transform mvp= projection * view * model;
    
    program_uniform(m_program, "mvpMatrix", mvp);
    program_uniform(m_program, "mvMatrix", mv);
    program_uniform(m_program, "normalMatrix", normal);
    
    // utiliser une texture, elle ne sera visible que si le mesh a des texcoords...
    program_use_texture(m_program, "diffuse_color", 0, texture); 
    
    if(m_indices.size() > 0)
        glDrawElements( m_primitives, (GLsizei) m_indices.size(), GL_UNSIGNED_INT, 0 );
    else
        glDrawArrays( m_primitives, 0, (GLsizei) m_positions.size() );
}


void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection, const GLuint texture )
{
    m.draw(model, view, projection, texture);
}

void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection )
{
    m.draw(model, view, projection, 0);
}
