
#version 430 

#ifdef VERTEX_SHADER

struct Triangle
{
	vec3 a;
	vec3 b;
	vec3 c;
};

layout(std430) readonly buffer bufferData 
{
	Triangle triangles[];
};

uniform mat4 mvpMatrix;

void main( )
{
	gl_Position= vec4(0, 0, 0, 1);
}
#endif


#ifdef FRAGMENT_SHADER

out vec4 color;

void main( )
{
	color= vec4(1, 1,  0, 1);
}
#endif
