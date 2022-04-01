
#version 330

#ifdef VERTEX_SHADER
uniform vec2 viewport;
uniform sampler2D positions;
uniform mat4 mvpMatrix;

void main( )
{
	ivec2 pixel= ivec2(gl_VertexID % int(viewport.x), gl_VertexID / int(viewport));
	gl_Position= mvpMatrix * texelFetch(positions, pixel, 0);
}
#endif

#ifdef FRAGMENT_SHADER
void main( )
{
	gl_FragColor= vec4(1, 0, 1, 1);
}
#endif
