
#version 430

#ifdef VERTEX_SHADER

in vec3 position;

uniform mat4 mvpMatrix;

void main( )
{
	gl_Position= mvpMatrix * vec4(position, 1);
}
#endif


#ifdef FRAGMENT_SHADER

layout(binding= 0, r32ui) coherent uniform uimage2D image;

out vec4 fragment_color;

void main( )
{
	uint n= imageAtomicAdd(image, ivec2(gl_FragCoord.xy), 1) +1;
	
	fragment_color= vec4(n/2, n/4, 1 - n/4, 1);
	//~ fragment_color= vec4(1, 0, 0, 1);
}
#endif
