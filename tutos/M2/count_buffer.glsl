
//! \file count_buffer.glsl filtrer des donnees / compacter des resultats, manipulation de compteurs atomiques partages.

#version 430

#ifdef COMPUTE_SHADER

layout( std430, binding= 0 ) buffer inputData
{
	int a[];
};

layout( std430, binding= 1 ) buffer outputData
{
	int b[];
};

layout( std430, binding= 2 ) buffer countData
{
	uint count;
};

uniform int v= 5;

shared uint group_count;
shared uint group_offset;

layout( local_size_x=256 ) in;
void main( )
{
	uint ID= gl_GlobalInvocationID.x;
	
/*	version directe...
	if(a[ID] < v)
	{
		uint index= atomicAdd(count, 1);
		b[index]= a[ID];
	}
*/

/*	version decomposee par groupe
 */
	// 0. init
	if(gl_LocalInvocationID.x == 0)
		group_count= 0;
	barrier();
	
	// 1. evaluer la condition / predicat
	uint offset= -1;
	if(a[ID] < v)
		offset= atomicAdd(group_count, 1);
	
	// group_count contient le nombre de threads qui veulent ecrire un resultat dans le buffer
	// et offset contient la position du resultat dans le groupe...
	
	// 2. reserver une place pour les resultats du groupe dans le buffer
	if(gl_LocalInvocationID.x == 0)
		group_offset= atomicAdd(count, group_count);
	barrier();
	// group_offset contient la position des resultats du groupe dans le buffer resultat
	
	// 3. ecrire les resultats
	if(offset != -1)
		// ecrit la valeur lorsqu'elle verifie la condition et qu'elle a une position...
		b[group_offset+offset]= a[ID];
		// toutes les valeurs d'un groupe sont ecrites en sequence dans le buffer resultat
}

#endif
