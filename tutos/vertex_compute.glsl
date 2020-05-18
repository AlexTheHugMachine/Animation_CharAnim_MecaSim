
//! \file vertex_compute.glsl

#version 430

#ifdef COMPUTE_SHADER

layout(std430, binding= 1) readonly buffer vertexData
{
    vec3 data[];
};

layout(std430, binding= 0) writeonly buffer transformedData
{
    vec4 transformed[];
};

uniform mat4 mvpMatrix;

layout(local_size_x= 256) in;
void main( )
{
    // chaque thread transforme un sommet.
    if(gl_LocalInvocationIndex < data.length())
        transformed[gl_GlobalInvocationID.x]= mvpMatrix * vec4(data[gl_GlobalInvocationID.x], 1);
}
#endif

