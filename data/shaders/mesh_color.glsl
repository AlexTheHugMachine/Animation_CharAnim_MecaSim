
#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;

uniform mat4 mvpMatrix;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
}
#endif

#ifdef FRAGMENT_SHADER
uniform vec4 mesh_color;

void main( )
{
    gl_FragColor= mesh_color;
}
#endif
