
#version 330

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;

layout(location= 0) in vec3 position;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
}

#endif


#ifdef FRAGMENT_SHADER

out vec4 fragment_color;

void main( )
{
    fragment_color= vec4(1, 0.4, 0, 1);
}

#endif
