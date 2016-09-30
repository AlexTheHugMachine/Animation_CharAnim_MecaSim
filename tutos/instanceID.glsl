
//! \file instanceID.glsl utilisation de glDrawArraysInstanced() et gl_InstanceID

#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 2) in vec3 normal;

uniform mat4 mvpMatrix;
uniform mat4 normalMatrix;

out vec3 vertex_normal;

void main( )
{
    // decale chaque instance sur l'axe x...
    vec3 p= position + vec3(gl_InstanceID * 4, 0, 0);
    gl_Position= mvpMatrix * vec4(p, 1);
    
    vertex_normal= mat3(normalMatrix) * normal;
}

// question : comment faire tourner chaque cube sur lui meme ? au lieu de faire tourner toute la "barre" ?

#endif

#ifdef FRAGMENT_SHADER
in vec3 vertex_normal;

out vec4 fragment_color;

void main( )
{
    fragment_color= vec4(abs(normalize(vertex_normal).zzz), 1);
}

#endif
