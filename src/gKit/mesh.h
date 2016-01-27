
#ifndef _MESH_H
#define _MESH_H

#include <vector>

#define GLEW_NO_GLU
#include "GL/glew.h"

#include "vec.h"


struct mesh
{
    std::vector<vec3> positions;
    std::vector<vec2> texcoords;
    std::vector<vec3> normals;
    std::vector<vec3> colors;
    
    std::vector<unsigned int> indices;
    
    GLenum primitives;
    int count;
};

mesh make_mesh( const GLenum primitives );

void mesh_texcoord( mesh& m, const vec2& texcoord );
void mesh_normal( mesh& m, const vec3& normal );
void mesh_color( mesh& m, const vec3& color );

unsigned int mesh_push_vertex( mesh& m, const vec3& position );

unsigned int mesh_push_ptn_vertex( mesh& m, const vec3& position, const vec2& texcoord, const vec3& normal );
unsigned int mesh_push_pt_vertex( mesh& m, const vec3& position, const vec2& texcoord );
unsigned int mesh_push_pn_vertex( mesh& m, const vec3& position, const vec3& normal );

void mesh_push_triangle( mesh& m, const unsigned int a, const unsigned int b, const unsigned int c );
void mesh_push_triangle_last( mesh& m, const int a, const int b, const int c );

void mesh_restart( mesh& m );

GLuint make_mesh_vertex_format( mesh& m );
GLuint make_mesh_program( mesh& m );

#endif
