
//! \file uniforms.glsl

#version 330 

#ifdef VERTEX_SHADER

struct Triangle
{
	vec3 a;
	vec3 b;
	vec3 c;
};

layout(std140) uniform bufferData 
{
	vec2 b;
	//~ float a;
	float c[3];
	//~ mat4 d;
};

layout(shared) uniform sharedData 
{
	vec2 sb;
	//~ float sa;
	float sc[3];
	//~ mat4 sd;
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
