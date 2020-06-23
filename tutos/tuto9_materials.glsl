//! \file tuto9_materials.glsl

#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 2) in vec3 normal;
layout(location= 4) in uint material;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

out vec3 vertex_position;
out vec3 vertex_normal;
flat out uint vertex_material;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    
    // position et normale dans le repere camera
    vertex_position= vec3(mvMatrix * vec4(position, 1));
    vertex_normal= mat3(mvMatrix) * normal;
    // indice de la matiere 
    vertex_material= material;
}

#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

in vec3 vertex_position;
in vec3 vertex_normal;
flat in uint vertex_material;

#define MAX_MATERIALS 16
uniform vec4 materials[MAX_MATERIALS];

void main( )
{
    vec3 l= normalize(-vertex_position);        // la camera est la source de lumiere.
    vec3 n= normalize(vertex_normal);
    float cos_theta= max(0, dot(n, l));
    fragment_color= materials[vertex_material] * cos_theta;
}

#endif
