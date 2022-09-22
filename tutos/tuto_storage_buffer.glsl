
#version 430

#ifdef VERTEX_SHADER

struct vertex
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
};

layout(std430, binding= 0) readonly buffer vertexData
{
	vertex data[];
};

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat4 normalMatrix;

out vec3 vertex_position;
out vec3 vertex_normal;
out vec2 vertex_texcoord;

void main( )
{
	vec3 p= data[gl_VertexID].position;
	vec3 n= data[gl_VertexID].normal;
	vec2 t=  data[gl_VertexID].texcoord;
	
	gl_Position= mvpMatrix * vec4(p, 1);
	
	vertex_position= vec3(mvMatrix * vec4(p, 1));
	vertex_normal= mat3(normalMatrix) * n;
	vertex_texcoord= t;
}
#endif

#ifdef FRAGMENT_SHADER

uniform sampler2D diffuse_color;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_texcoord;

out vec4 fragment_color;

void main( )
{
	float cos_theta= dot(normalize(vertex_normal), normalize(-vertex_position));
	vec3 color= texture(diffuse_color, vertex_texcoord).rgb * cos_theta;
	fragment_color= vec4(color, 1);
}
#endif

