
#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 2) in vec3 normal;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

out vec3 vertex_position;
out vec3 vertex_normal;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    
    vertex_position= vec3(mvMatrix * vec4(position, 1));
    vertex_normal= mat3(mvMatrix) * normal;
}
#endif

#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

in vec3 vertex_position;
in vec3 vertex_normal;

void main( )
{
    vec3 l= normalize(-vertex_position);        // la camera est la source de lumiere.
    vec3 n= normalize(vertex_normal);
    float cos_theta= max(0, dot(n, l));
    vec3 color= vec3(0.8) * cos_theta;

    fragment_color= vec4(color, 1);
}
#endif
