
#ifndef _MESH_H
#define _MESH_H

#include <vector>

#define GLEW_NO_GLU
#include "GL/glew.h"

#include "vec.h"
#include "mat.h"


struct Mesh
{
    std::vector<float> positions;       // vec3
    std::vector<float> texcoords;       // vec2
    std::vector<float> normals;         // vec3
    std::vector<float> colors;          // vec3
    
    std::vector<unsigned int> indices;
    
    GLenum primitives;
    int count;
    GLuint vao;
    GLuint program;
};

Mesh make_mesh( const GLenum primitives );
void release_mesh( Mesh& m );

void vertex_texcoord( Mesh& m, const float u, const float v );
void vertex_normal( Mesh& m, const vec3& normal );
void vertex_color( Mesh& m, const vec3& color );

unsigned int push_vertex( Mesh& m, const vec3& position );

unsigned int push_vertex( Mesh& m, const vec3& position, const float u, const float v, const vec3& normal );
unsigned int push_vertex( Mesh& m, const vec3& position, const float u, const float v );
unsigned int push_vertex( Mesh& m, const vec3& position, const vec3& normal );

void push_triangle( Mesh& m, const unsigned int a, const unsigned int b, const unsigned int c );
void push_triangle_last( Mesh& m, const int a, const int b, const int c );

void restart_strip( Mesh& m );

void bounds( const Mesh& m, vec3& pmin, vec3& pmax );

GLuint make_mesh_vertex_format( Mesh& m );
GLuint make_mesh_program( Mesh& m );

void draw( Mesh& m, const mat4& model, const mat4& view, const mat4& projection );
void draw( Mesh& m, const mat4& model, const mat4& view, const mat4& projection, GLuint texture );

#endif
