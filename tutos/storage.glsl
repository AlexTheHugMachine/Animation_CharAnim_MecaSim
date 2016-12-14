
#version 430 

#ifdef VERTEX_SHADER

struct Triangle
{
	//~ vec3 a;
	//~ vec3 ab;
	//~ vec3 ac;
	float a[2];
	int q[2];
};

//~ layout(std430) readonly buffer bufferData 
//~ {
	//~ Triangle triangles[];
//~ };

layout(std430) buffer bufferData 
{
	Triangle positions[];
};

uniform mat4 mvpMatrix;

//~ layout(local_size_x= 64) in;
void main( )
{
	//~ gl_Position= mvpMatrix * vec4(positions[gl_VertexID].a, 1);
	//~ vec4 p;
	//~ for(int i= 0 ; i< positions.length(); i++)
		//~ p= p + vec4(positions[i], 1);
		
	//~ gl_Position= mvpMatrix * p;
}

#endif

#ifdef FRAGMENT_SHADER
out vec4 color;
void main( )
{
	color= vec4(1, 1,  0, 1);
}

#endif
