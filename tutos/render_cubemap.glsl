
//! \file render_cubemap.glsl 

#version 330

#extension GL_ARB_shader_viewport_layer_array : require

#ifdef VERTEX_SHADER
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix[6];

in vec3 position;
in vec3 normal;
out vec3 vertex_normal;

void main( )
{
    gl_Position= projectionMatrix * viewMatrix[gl_InstanceID] * modelMatrix * vec4(position, 1);
    gl_Layer= gl_InstanceID;
    
    vertex_normal= mat3(modelMatrix) * normal;
}
#endif


#ifdef FRAGMENT_SHADER
in vec3 vertex_normal;
out vec4 fragment_color;

void main( )
{
    vec4 color= vec4(abs(vertex_normal) * length(gl_FragCoord.xy / 1024) * 8, 1);
    fragment_color= color;
}
#endif
