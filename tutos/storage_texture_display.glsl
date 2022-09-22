
#version 430

#ifdef VERTEX_SHADER

void main( )
{
	vec3 positions[3]= vec3[3]( vec3(-1, 1, -1), vec3( -1, -3, -1), vec3( 3,  1, -1) );

	gl_Position= vec4(positions[gl_VertexID], 1.0);
}
#endif


#ifdef FRAGMENT_SHADER

layout(binding= 0, r32ui) readonly uniform uimage2D image;

out vec4 fragment_color;

void main( )
{
	uint n= imageLoad(image, ivec2(gl_FragCoord.xy)).r;
	
	fragment_color= vec4(n/2, n/4, 1 - n/4, 1);
}
#endif
