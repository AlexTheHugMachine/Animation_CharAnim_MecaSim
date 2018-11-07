
//! \file indirect_remap.glsl

#version 430

#ifdef VERTEX_SHADER

#extension GL_ARB_shader_draw_parameters : require

layout(location= 0) in vec3 position;
out vec3 vertex_position;

uniform mat4 modelMatrix;
uniform mat4 vpMatrix;
uniform mat4 viewMatrix;

// row_major : organisation des matrices par lignes...
layout(binding= 0, row_major, std430) readonly buffer modelData
{
    mat4 objectMatrix[];
};

layout(binding= 1, std430) readonly buffer remapData
{
    uint remap[];
};

void main( )
{
    uint id= remap[gl_DrawIDARB];
    gl_Position= vpMatrix * objectMatrix[id] * modelMatrix * vec4(position, 1);
    
    // position dans le repere camera
    vertex_position= vec3(viewMatrix * objectMatrix[id] * modelMatrix * vec4(position, 1));
}
#endif


#ifdef FRAGMENT_SHADER

in vec3 vertex_position;

out vec4 fragment_color;

void main( )
{
    vec4 color= vec4(1, 0.8, 0, 1);
    // recalcule la normale geometrique du triangle, dans le repere camera
    vec3 t= normalize(dFdx(vertex_position));
    vec3 b= normalize(dFdy(vertex_position));
    vec3 normal= normalize(cross(t, b));
    
    // matiere diffuse...
    float cos_theta= max(0.0, normal.z);
    color= color * cos_theta;
    
    fragment_color= vec4(color.rgb, 1);
}
#endif
