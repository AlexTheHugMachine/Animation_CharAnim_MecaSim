
//! \file render_cubemap.glsl 

#version 330

#extension GL_ARB_shader_viewport_layer_array : require

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix[6];
uniform mat4 modelMatrix;

layout(location= 0) in vec3 position;
layout(location= 2) in vec3 normal;

out vec3 vertex_position;
out vec3 vertex_normal;

void main( )
{
    gl_Position= mvpMatrix[gl_InstanceID] * vec4(position, 1);
    gl_Layer= gl_InstanceID;
    
    vertex_position= vec3(modelMatrix * vec4(position, 1));
    vertex_normal= mat3(modelMatrix) * normal;
}
#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

in vec3 vertex_position;
in vec3 vertex_normal;
void main( )
{
    fragment_color= vec4(abs(vertex_position / 20), 1);
}
#endif
