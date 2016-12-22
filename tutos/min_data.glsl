
#version 430

#ifdef COMPUTE_SHADER

layout(binding= 0, std430) readonly buffer inputData 
{
	int data[];
};

layout(binding= 1, std430) coherent buffer tmpData 
{
	int tmp[];
};

uniform uint N;
uniform uint n;

layout(local_size_x= 1024) in;
void main( )
{
	uint id= gl_GlobalInvocationID.x;
	
	if(id < N/2)
		tmp[N/2+id]= min(data[id*2], data[id*2+1]);
	
	if(id < n/2)
		tmp[n/2+id]= min(tmp[n+id*2], tmp[n+id*2+1]);
}

#endif

