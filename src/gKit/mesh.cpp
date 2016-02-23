
#include <cstdio>
#include <string>

#include "program.h"
#include "buffer.h"
#include "mesh.h"


Mesh create_mesh( const GLenum primitives )
{
    Mesh m;
    m.primitives= primitives;
    m.count= 0;
    m.vao= 0;
    m.program= 0;
    return m;
}

void release_mesh( Mesh& m )
{
    if(m.vao)
        release_vertex_format(m.vao);
    if(m.program)
        glDeleteProgram(m.program);
    return;
}

void vertex_texcoord( Mesh& m, const vec2& uv )
{
    m.texcoords.push_back(uv);
}

void vertex_texcoord( Mesh& m, const float u, const float v )
{
    vertex_texcoord(m, make_vec2(u, v));
}

void vertex_normal( Mesh& m, const vec3& normal )
{
    m.normals.push_back(normal);
}

void vertex_normal( Mesh& m, const Vector& n )
{
    vertex_normal(m, make_vec3(n.x, n.y, n.z));
}

void vertex_color( Mesh& m, const vec3& color )
{
    m.colors.push_back(color);
}

void vertex_color( Mesh& m, const Color& color )
{
    vertex_color(m, make_vec3(color.r, color.g, color.b));
}

unsigned int push_vertex( Mesh& m, const Point& p )
{
    return push_vertex(m, make_vec3(p.x, p.y, p.z));
}

unsigned int push_vertex( Mesh& m, const vec3& position )
{
    m.positions.push_back(position);

    if(m.texcoords.size() > 0 && m.texcoords.size() != m.positions.size())
        m.texcoords.push_back(m.texcoords.back());
    if(m.normals.size() > 0 && m.normals.size() != m.positions.size())
        m.normals.push_back(m.normals.back());
    if(m.colors.size() > 0 && m.colors.size() != m.positions.size())
        m.colors.push_back(m.colors.back());
    
    unsigned int index= (unsigned int) m.positions.size() -1;
    // construction de l'index buffer pour les strip
    bool strip= false;
    switch(m.primitives)
    {
        case GL_LINE_STRIP:
        case GL_LINE_LOOP:
        case GL_TRIANGLE_STRIP:
        case GL_TRIANGLE_FAN:
            m.indices.push_back(index);
            break;
        default:
            break;
    }
    
    // renvoie l'indice du sommet
    return index;
}


unsigned int push_vertex( Mesh& m, const vec3& position, const float u, const float v, const vec3& normal )
{
    vertex_texcoord(m, u, v);
    vertex_normal(m, normal);
    return push_vertex(m, position);
}

unsigned int push_vertex( Mesh& m, const Point& position, const float u, const float v, const Vector& normal )
{
    vertex_texcoord(m, u, v);
    vertex_normal(m, normal);
    return push_vertex(m, position);
}

unsigned int push_vertex( Mesh& m, const vec3& position, const float u, const float v )
{
    vertex_texcoord(m, u ,v);
    return push_vertex(m, position);
}

unsigned int push_vertex( Mesh& m, const Point& position, const float u, const float v )
{
    vertex_texcoord(m, u ,v);
    return push_vertex(m, position);
}

unsigned int push_vertex( Mesh& m, const vec3& position, const vec3& normal )
{
    vertex_normal(m, normal);
    return push_vertex(m, position);
}

unsigned int push_vertex( Mesh& m, const Point& position, const Vector& normal )
{
    vertex_normal(m, normal);
    return push_vertex(m, position);
}

void push_triangle( Mesh& m, const unsigned int a, const unsigned int b, const unsigned int c )
{
    m.indices.push_back(a);
    m.indices.push_back(b);
    m.indices.push_back(c);
}

void push_triangle_last( Mesh& m, const int a, const int b, const int c )
{
    // a, b, et c doivent etre negatifs...
    if(a >= 0) return;
    if(b >= 0) return;
    if(c >= 0) return;

    m.indices.push_back((int) m.positions.size() + a);
    m.indices.push_back((int) m.positions.size() + b);
    m.indices.push_back((int) m.positions.size() + c);
}

void restart_strip( Mesh& m )
{
    m.indices.push_back(~0u);   // ~0u plus grand entier non signe representable.
#if 1
    glPrimitiveRestartIndex(~0u);
    glEnable(GL_PRIMITIVE_RESTART);
#else
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX); // n'existe pas sur mac ?!
#endif
}

void bounds( const Mesh& m, Point& pmin, Point& pmax )
{
    if(m.positions.size() < 1)
        return;
    
    pmin= make_point(m.positions[0].x, m.positions[0].y, m.positions[0].z);
    pmax= pmin; 
    
    for(unsigned int i= 1; i < (unsigned int) m.positions.size(); i++)
    {
        vec3 p= m.positions[i];
        pmin= make_point( std::min(pmin.x, p.x), std::min(pmin.y, p.y), std::min(pmin.z, p.z) );
        pmax= make_point( std::max(pmax.x, p.x), std::max(pmax.y, p.y), std::max(pmax.z, p.z) );
    }
}


GLuint make_mesh_vertex_format( Mesh& m )
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
    GLuint vao= create_vertex_format();
    make_vertex_buffer(vao, 0,  3, GL_FLOAT, m.positions.size() * sizeof(vec3), &m.positions.front());
    if(m.texcoords.size()== m.positions.size())
        make_vertex_buffer(vao, 1,  2, GL_FLOAT, m.texcoords.size() * sizeof(vec2), &m.texcoords.front());
    if(m.normals.size() == m.positions.size())
        make_vertex_buffer(vao, 2,  3, GL_FLOAT, m.normals.size() * sizeof(vec3), &m.normals.front());
    if(m.colors.size() == m.positions.size())
        make_vertex_buffer(vao, 3,  3, GL_FLOAT, m.colors.size() * sizeof(vec3), &m.colors.front());

    if(m.indices.size() > 0)
    {
        // conserver le nombre d'indices de sommets pour glDrawElements( )
        m.count= (int) m.indices.size();
        make_index_buffer(vao, m.indices.size() * sizeof(unsigned int), &m.indices.front());
    }

    return vao;
}

GLuint make_mesh_program( Mesh& m )
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


void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection, GLuint texture )
{
    if(m.vao == 0)
        m.vao= make_mesh_vertex_format(m);
    if(m.program == 0)
        m.program= make_mesh_program(m);
    
    glBindVertexArray(m.vao);
    glUseProgram(m.program);
    
    Transform mv= view * model;
    Transform normal= make_normal_transform(mv);
    Transform mvp= projection * view * model;
    
    program_uniform(m.program, "mvpMatrix", mvp);
    program_uniform(m.program, "mvMatrix", mv);
    program_uniform(m.program, "normalMatrix", normal);

    // utiliser une texture, elle ne sera visible que si le mesh a des texcoords...
    if(texture > 0)
        program_use_texture(m.program, "diffuse_color", 0, texture); 

    if(m.indices.size() > 0)
        glDrawElements(m.primitives, m.count, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(m.primitives, 0, m.count);
}

void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection )
{
    draw(m, model, view, projection, 0);
}
