
#version 330

#ifdef VERTEX_SHADER
in vec3 position;

uniform mat4 mvpMatrix;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
}
#endif

#ifdef FRAGMENT_SHADER
uniform vec3 color;

void main( )
{
    gl_FragColor= vec4(color, 1);
}
#endif
