
//! \file tuto5GL_cubemap.glsl reflechir une cubemap sur un objet

#version 430

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;

layout(location= 0) in vec3 position;
layout(location= 2) in vec3 normal;
out vec3 vertex_position;
out vec3 vertex_normal;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_position= vec3(modelMatrix * vec4(position, 1));
    vertex_normal= mat3(modelMatrix) * normal;
}
#endif


#ifdef FRAGMENT_SHADER
uniform vec3 camera_position;
uniform samplerCube texture0;

const float alpha= 22;
const float k= 0.8;

in vec3 vertex_position;
in vec3 vertex_normal;
out vec4 fragment_color;

void main( )
{
    vec3 v= vertex_position - camera_position;
    vec3 n= normalize(vertex_normal);
    vec3 m= reflect(v, n);
    //~ vec3 color= texture(texture0, m).rgb;
    vec3 color= texture(texture0, n).rgb;
    
    //~ // approximation
    //~ float w= textureSize(texture0, 0).x;
    //~ float dlevel= log2(w) +1;
    //~ float glevel= max(0, log2(w * sqrt(3)) - 0.5 * log2(alpha+1));
    //~ vec3 diffuse= textureLod(texture0, n, dlevel).rgb;
    //~ vec3 glossy= textureLod(texture0, m, glevel).rgb;
    //~ vec3 color= k * diffuse + (1 - k) * glossy;
    
    fragment_color= vec4(color, 1);
}
#endif
