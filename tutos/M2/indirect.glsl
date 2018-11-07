
//! \file indirect.glsl

#version 430

#ifdef VERTEX_SHADER

#extension GL_ARB_shader_draw_parameters : require

layout(location= 0) in vec3 position;
out vec3 vertex_position;

uniform mat4 modelMatrix;
uniform mat4 vpMatrix;
uniform mat4 viewMatrix;

uniform mat4 model[1000];

void main( )
{
    gl_Position= vpMatrix * model[gl_DrawIDARB] * modelMatrix * vec4(position, 1);
    vertex_position= vec3(viewMatrix * model[gl_DrawIDARB] * modelMatrix * vec4(position, 1));
}
#endif


#ifdef FRAGMENT_SHADER

in vec3 vertex_position;

out vec4 fragment_color;

void main( )
{
    vec4 color= vec4(1, 0.8, 0, 1);
    // recalcule la normale geometrique du triangle
    vec3 t= normalize(dFdx(vertex_position));
    vec3 b= normalize(dFdy(vertex_position));
    vec3 normal= normalize(cross(t, b));
    
    // matiere diffuse...
    float cos_theta= max(0.0, normal.z);
    color= color * cos_theta;
    
    fragment_color= vec4(color.rgb, 1);
}
#endif
