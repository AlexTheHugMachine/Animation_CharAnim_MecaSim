
//! \file tuto5GL_cubemap.glsl reflechir une cubemap sur un objet

#version 330

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;
uniform mat4 mMatrix;

in vec3 position;
in vec3 normal;
out vec3 vertex_position;
out vec3 vertex_normal;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_position= vec3(mMatrix * vec4(position, 1));
    vertex_normal= mat3(mMatrix) * normal;
}
#endif


#ifdef FRAGMENT_SHADER
uniform vec3 camera_position;
uniform samplerCube texture0;

in vec3 vertex_position;
in vec3 vertex_normal;
out vec4 fragment_color;

void main( )
{
    vec3 m= reflect(normalize(vertex_position - camera_position), normalize(vertex_normal));
    vec4 color= texture(texture0, m);
    
    fragment_color= color;
}
#endif
