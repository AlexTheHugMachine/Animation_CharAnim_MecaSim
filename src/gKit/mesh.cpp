
#include <cstdio>
#include <string>

#include "program.h"
#include "buffer.h"
#include "mesh.h"


mesh make_mesh( const GLenum primitives )
{
    mesh m;
    m.primitives= primitives;
    m.count= 0;
    m.vao= 0;
    m.program= 0;
    return m;
}

void release_mesh( mesh& m )
{
    if(m.vao)
        glDeleteVertexArrays(1, &m.vao);
    if(m.program)
        glDeleteProgram(m.program);
    return;
}

void vertex_texcoord( mesh& m, const float u, const float v )
{
    m.texcoords.push_back(u);
    m.texcoords.push_back(v);
}

void vertex_normal( mesh& m, const vec3& normal )
{
    m.normals.push_back(normal.x);
    m.normals.push_back(normal.y);
    m.normals.push_back(normal.z);
}

void vertex_color( mesh& m, const vec3& color )
{
    m.colors.push_back(color.x);
    m.colors.push_back(color.y);
    m.colors.push_back(color.z);
}

unsigned int push_vertex( mesh& m, const vec3& position )
{
    m.positions.push_back(position.x);
    m.positions.push_back(position.y);
    m.positions.push_back(position.z);

    if(m.texcoords.size() > 0 && m.texcoords.size() / 2 != m.positions.size() / 3)
    {
        size_t attrib= m.texcoords.size();
        float u= m.texcoords[attrib -2];
        float v= m.texcoords[attrib -1];
        vertex_texcoord(m, u, v);
    }

    if(m.normals.size() > 0 && m.normals.size() / 3 != m.positions.size() / 3)
    {
        size_t attrib= m.normals.size();
        float x= m.normals[attrib -3];
        float y= m.normals[attrib -2];
        float z= m.normals[attrib -1];
        vertex_normal(m, make_vec3(x, y, z));
}

    if(m.colors.size() > 0 && m.colors.size() / 3 != m.positions.size() / 3)
    {
        size_t attrib= m.colors.size();
        float r= m.colors[attrib -3];
        float g= m.colors[attrib -2];
        float b= m.colors[attrib -1];
        vertex_color(m, make_vec3(r, g, b));
    }

    // construction de l'index buffer pour les strip
    bool strip= false;
    switch(m.primitives)
{
        case GL_LINE_STRIP:
        case GL_LINE_LOOP:
        case GL_TRIANGLE_STRIP:
        case GL_TRIANGLE_FAN:
            strip= true;
            break;
        default:
            strip= false;
}

    unsigned int index= ((unsigned int) m.positions.size() - 3) / 3;
    if(strip)
        m.indices.push_back(index);
    
    // renvoie l'indice du sommet
    return index;
}


unsigned int push_vertex( mesh& m, const vec3& position, const float u, const float v, const vec3& normal )
{
    vertex_texcoord(m, u, v);
    vertex_normal(m, normal);
    return push_vertex(m, position);
}

unsigned int push_vertex( mesh& m, const vec3& position, const float u, const float v )
{
    vertex_texcoord(m, u ,v);
    return push_vertex(m, position);
}

unsigned int push_vertex( mesh& m, const vec3& position, const vec3& normal )
{
    vertex_normal(m, normal);
    return push_vertex(m, position);
}

void push_triangle( mesh& m, const unsigned int a, const unsigned int b, const unsigned int c )
{
    m.indices.push_back(a);
    m.indices.push_back(b);
    m.indices.push_back(c);
}

void push_triangle_last( mesh& m, const int a, const int b, const int c )
{
    // a, b, et c doivent etre negatifs...
    if(a >= 0) return;
    if(b >= 0) return;
    if(c >= 0) return;

    m.indices.push_back((int) m.positions.size() + a);
    m.indices.push_back((int) m.positions.size() + b);
    m.indices.push_back((int) m.positions.size() + c);
}

void restart_strip( mesh& m )
{
    m.indices.push_back(~0u);   // ~0u plus grand entier non signe representable.
#if 0
    glPrimitiveRestartIndex(~0u);
    glEnable(GL_PRIMITIVE_RESTART);
#else
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
#endif
}

void bounds( const mesh& m, vec3& pmin, vec3& pmax )
{
    if(m.positions.size() < 3)
        return;
    
    pmin= make_vec3(m.positions[0], m.positions[1], m.positions[2]);
    pmax= pmin; 
    
    for(unsigned int i= 3; i < (unsigned int) m.positions.size(); i+= 3)
    {
        float x= m.positions[i];
        float y= m.positions[i +1];
        float z= m.positions[i +2];
        pmin= make_vec3( std::min(pmin.x, x), std::min(pmin.y,  y), std::min(pmin.z, z) );
        pmax= make_vec3( std::max(pmax.x, x), std::max(pmax.y,  y), std::max(pmax.z, z) );
    }
}


GLuint make_mesh_vertex_format( mesh& m )
{
    if(m.positions.size() == 0)
        return 0;

#if 1
    if(m.texcoords.size() > 0 && m.texcoords.size() / 2 < m.positions.size() / 3)
        printf("[error] invalid texcoords array...\n");
    if(m.normals.size() > 0 && m.normals.size() / 3 < m.positions.size() / 3)
        printf("[error] invalid normals array...\n");
    if(m.colors.size() > 0 && m.colors.size() / 3 < m.positions.size() / 3)
        printf("[error] invalid colors array...\n");
#endif

    // conserver le nombre de sommets pour glDrawArrays( )
    m.count = (int) m.positions.size() / 3;

    // ne creer que les buffers necessaires
    GLuint vao= make_vertex_format();
    make_vertex_buffer(vao, 0,  3, GL_FLOAT, m.positions.size() * sizeof(float), &m.positions.front());
    if(m.texcoords.size() / 2 == m.positions.size() / 3)
        make_vertex_buffer(vao, 1,  2, GL_FLOAT, m.texcoords.size() * sizeof(float), &m.texcoords.front());
    if(m.normals.size() / 3 == m.positions.size() /3)
        make_vertex_buffer(vao, 2,  3, GL_FLOAT, m.normals.size() * sizeof(float), &m.normals.front());
    if(m.colors.size() / 3 == m.positions.size() / 3)
        make_vertex_buffer(vao, 3,  3, GL_FLOAT, m.colors.size() * sizeof(float), &m.colors.front());

    if(m.indices.size() > 0)
    {
        // conserver le nombre d'indices de sommets pour glDrawElements( )
        m.count= m.indices.size();
        make_index_buffer(vao, m.indices.size() * sizeof(unsigned int), &m.indices.front());
    }

    return vao;
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

    return read_program_definitions("data/shaders/mesh.glsl", definitions.c_str());
}


void draw( mesh& m, const mat4& model, const mat4& view, const mat4& projection, GLuint texture )
{
    if(m.vao == 0)
        m.vao= make_mesh_vertex_format(m);
    if(m.program == 0)
        m.program= make_mesh_program(m);
    
    glBindVertexArray(m.vao);
    glUseProgram(m.program);
    
    mat4 mv= view * model;
    mat4 normal= make_normal_matrix(mv);
    mat4 mvp= projection * view * model;
    
    program_set_mat4(m.program, "mvpMatrix", mvp);
    program_set_mat4(m.program, "mvMatrix", mv);
    program_set_mat4(m.program, "normalMatrix", normal);

    // utiliser une texture, elle ne sera visible que si le mesh a des texcoords...
    if(texture > 0)
        program_use_texture(m.program, "diffuse_color", 0, texture); 

    if(m.indices.size() > 0)
        glDrawElements(m.primitives, m.count, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(m.primitives, 0, m.count);
}

void draw( mesh& m, const mat4& model, const mat4& view, const mat4& projection )
{
    draw(m, model, view, projection, 0);
}
