
//! \file instance_buffer.glsl utilisation de glDrawArraysInstanced() et attributs d'instance 

#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 2) in vec3 normal;

// l'attribut d'instance se declare "normalement", c'est l'application qui doit correctement configurer le vao...
layout(location= 1) in vec3 instance_position;

uniform mat4 mvpMatrix;
uniform mat4 normalMatrix;

out vec3 vertex_normal;

void main( )
{
    vec3 p= position + instance_position;
    gl_Position= mvpMatrix * vec4(p, 1);
    
    vertex_normal= mat3(normalMatrix) * normal;
}
#endif

#ifdef FRAGMENT_SHADER
in vec3 vertex_normal;

out vec4 fragment_color;

void main( )
{
    fragment_color= vec4(abs(normalize(vertex_normal).zzz), 1);
}

#endif
