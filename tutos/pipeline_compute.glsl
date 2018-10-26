
//! \file pipeline_compute.glsl

#version 430

#ifdef VERTEX_SHADER

layout(std430, binding= 0) readonly buffer transformedData
{
    vec4 transformed[];
};

void main( )
{
    gl_Position= transformed[gl_VertexID];
}
#endif

#ifdef FRAGMENT_SHADER

out vec4 fragment_color;

void main( )
{
    fragment_color= vec4(1, 1, 0, 1);
}
#endif

