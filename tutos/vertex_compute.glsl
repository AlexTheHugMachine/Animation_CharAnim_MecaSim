
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
    // recupere l'indice du thread
    const uint ID= gl_GlobalInvocationID.x;
    
    // chaque thread transforme le sommet d'indice ID...
    if(ID < data.length())
        transformed[ID]= mvpMatrix * vec4(data[ID], 1);
}
#endif

