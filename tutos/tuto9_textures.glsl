
//! \file tuto9_textures.glsl

#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoord;

uniform mat4 mvpMatrix;

out vec2 vertex_texcoord;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_texcoord= texcoord;
}

#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

in vec2 vertex_texcoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main( )
{
    vec4 color0= texture(texture0, vertex_texcoord);
    vec4 color1= texture(texture1, vertex_texcoord);
    
    fragment_color= color0 * color1;
}

#endif
