
#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 2) in vec3 normal;

uniform mat4 mvpMatrix;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
}
#endif

#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

void main( )
{
    fragment_color= vec4(0,0,0, 1); // dessine l'objet en noir
}
#endif
