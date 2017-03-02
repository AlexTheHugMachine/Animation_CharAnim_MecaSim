
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

#if 1
shared int min_group;

layout(local_size_x= 1024) in;
void main( )
{
	uint id= gl_GlobalInvocationID.x;
	
	if(gl_LocalInvocationID.x == 0)
		min_group= 1000000;	// max int
	
	barrier();
	memoryBarrierShared();
	
	if(id < N)
		atomicMin(min_group, data[id]);
		
	if(gl_LocalInvocationID.x == 0)
		atomicMin(tmp[1], min_group);
}
#else
layout(local_size_x= 1024) in;
void main( )
{
	uint id= gl_GlobalInvocationID.x;
	
	if(id < N)
		atomicMin(tmp[1], data[id]);
}
#endif

#endif

