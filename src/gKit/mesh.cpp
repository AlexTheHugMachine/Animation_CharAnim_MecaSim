
#include <stdio.h>
#include <string>

#include "program.h"
#include "buffer.h"
#include "mesh.h"


mesh make_mesh( const GLenum primitives )
{
    mesh m;
    m.primitives= primitives;
    m.count= 0;
    return m;
}


void mesh_texcoord( mesh& m, const vec2& texcoord )
{
    m.texcoords.push_back(texcoord);
}

void mesh_normal( mesh& m, const vec3& normal )
{
    m.normals.push_back(normal);
}

void mesh_color( mesh& m, const vec3& color )
{
    m.colors.push_back(color);
}

unsigned int mesh_push_vertex( mesh& m, const vec3& position )
{
    m.positions.push_back(position);

    if(m.texcoords.size() > 0 && m.texcoords.size() != m.positions.size())
        m.texcoords.push_back(m.texcoords.back());
    if(m.normals.size() > 0 && m.normals.size() != m.positions.size())
        m.normals.push_back(m.normals.back());
    if(m.colors.size() > 0 && m.colors.size() != m.positions.size())
        m.colors.push_back(m.colors.back());

    return m.positions.size() -1;
}


unsigned int mesh_push_ptn_vertex( mesh& m, const vec3& position, const vec2& texcoord, const vec3& normal )
{
    m.texcoords.push_back(texcoord);
    m.normals.push_back(normal);
    return mesh_push_vertex(m, position);
}

unsigned int mesh_push_pt_vertex( mesh& m, const vec3& position, const vec2& texcoord )
{
    m.texcoords.push_back(texcoord);
    return mesh_push_vertex(m, position);
}

unsigned int mesh_push_pn_vertex( mesh& m, const vec3& position, const vec3& normal )
{
    m.normals.push_back(normal);
    return mesh_push_vertex(m, position);
}


void mesh_push_triangle( mesh& m, const unsigned int a, const unsigned int b, const unsigned int c )
{
    m.indices.push_back(a);
    m.indices.push_back(b);
    m.indices.push_back(c);
}

void mesh_push_triangle_last( mesh& m, const int a, const int b, const int c )
{
    // a, b, et c doivent etre negatifs...
    if(a >= 0) return;
    if(b >= 0) return;
    if(c >= 0) return;

    m.indices.push_back(m.positions.size() + a);
    m.indices.push_back(m.positions.size() + b);
    m.indices.push_back(m.positions.size() + c);
}

void mesh_restart( mesh& m )
{
    m.indices.push_back(~0u);   // ~0u plus grand entier non signe representable.
#if 0
    glPrimitiveRestartIndex(~0u);
    glEnable(GL_PRIMITIVE_RESTART);
#else
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
#endif
}



GLuint make_mesh_vertex_format( mesh& m )
{
    if(m.positions.size() == 0)
        return 0;

#if 1
    if(m.texcoords.size() > 0 && m.texcoords.size() < m.positions.size())
        printf("[error] invalid texcoords array...\n");
    if(m.normals.size() > 0 && m.normals.size() < m.positions.size())
        printf("[error] invalid normals array...\n");
    if(m.colors.size() > 0 && m.colors.size() < m.positions.size())
        printf("[error] invalid colors array...\n");
#endif

    // conserver le nombre de sommets pour glDrawArrays( )
    m.count = (int) m.positions.size();

    // ne creer que les buffers necessaires
    GLuint cube_vao= make_vertex_format();
    make_vertex_buffer(cube_vao, 0, 3, GL_FLOAT, m.positions.size() * sizeof(vec3), &m.positions.front());
    if(m.texcoords.size() == m.positions.size())
        make_vertex_buffer(cube_vao, 1, 2, GL_FLOAT, m.texcoords.size() * sizeof(vec2), &m.texcoords.front());
    if(m.normals.size() == m.positions.size())
        make_vertex_buffer(cube_vao, 2, 3, GL_FLOAT, m.normals.size() * sizeof(vec3), &m.normals.front());
    if(m.colors.size() == m.positions.size())
        make_vertex_buffer(cube_vao, 3, 3, GL_FLOAT, m.colors.size() * sizeof(vec3), &m.colors.front());

    if(m.indices.size() > 0)
    {
        // conserver le nombre d'indices de sommets pour glDrawElements( )
        m.count= m.indices.size();
        make_index_buffer(cube_vao, m.indices.size() * sizeof(unsigned int), &m.indices.front());
    }

    return cube_vao;
}

GLuint make_mesh_program( mesh& m )
{
    std::string definitions;

    if(m.texcoords.size() > 0)
        definitions.append("#define USE_TEXCOORD\n");
    if(m.normals.size() > 0)
        definitions.append("#define USE_NORMAL\n");
    if(m.colors.size() > 0)
        definitions.append("#define USE_COLOR\n");

    return read_program_definitions("src/shader/vertex.glsl", "src/shader/fragment.glsl", definitions.c_str());
}


