#version 430
#extension GL_ARB_shader_draw_parameters : require

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 position;
out vec3 vertex_position;

uniform mat4 vpMatrix;
uniform mat4 viewMatrix;

#if 1
layout(row_major) uniform modelBuffer
{
    mat4 model[25];
};
#else
uniform mat4 model[25];
#endif

void main( )
{
    gl_Position= vpMatrix * model[gl_DrawIDARB] * vec4(position, 1);
    vertex_position= vec3(viewMatrix * model[gl_DrawIDARB] * vec4(position, 1));
}
#endif


#ifdef FRAGMENT_SHADER

in vec3 vertex_position;

out vec4 fragment_color;

void main( )
{
    vec4 color= vec4(1, 0.8, 0, 1);
    vec3 t= normalize(dFdx(vertex_position));
    vec3 b= normalize(dFdy(vertex_position));
    vec3 normal= normalize(cross(t, b));
    
    float cos_theta= abs(normal.z);
    color= color * cos_theta;
    
    fragment_color= vec4(color.rgb, 1);
}
#endif
