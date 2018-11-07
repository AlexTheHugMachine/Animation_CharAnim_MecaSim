
//! \file indirect_cull.glsl

#version 430

#ifdef COMPUTE_SHADER

struct Object
{
    vec3 pmin;
    uint vertex_count;
    vec3 pmax;
    uint vertex_base;
};

struct Draw
{
    uint vertex_count;
    uint instance_count;
    uint vertex_base;
    uint instance_base;
};
    
layout(binding= 0, std430) readonly buffer objectData
{
    Object objects[];
};

layout(binding= 1, std430) writeonly buffer remapData
{
    uint remap[];
};

layout(binding= 2, std430) writeonly buffer paramData
{
    Draw params[];
};

layout(binding= 3) buffer counterData
{
    uint count;
};


uniform vec3 bmin;
uniform vec3 bmax;


layout(local_size_x= 256) in;
void main( )
{
    uint id= gl_GlobalInvocationID.x;
    if(id > objects.length())
        return;
    
    // recupere la bbox du ieme objet...
    vec3 pmin= objects[id].pmin;
    vec3 pmax= objects[id].pmax;
    
    // test d'inclusion avec la bbox...
    if(any(lessThan(pmax, bmin))        // trop a gauche pour x, etc
    || any(greaterThan(pmin, bmax)))    // trop a droite pour x, etc
        // pas d'intersection...
        return;
    
    // bvec3 lessThan( vec3 a, vec3 b ) : compare les vecteurs composante par composante et renvoie un vecteur de bool.
    // bvec3 greaterThan( vec3 a, vec3 b ) : idem.
    // bool any( bvec3 ) : renvoie vrai si une des composantes du vecteur est vraie.   
    
    // l'objet est dans la bbox, il faut le dessiner : emettre les parametres du draw
    // etape 1 : position dans le buffer de sortie
    uint index= atomicAdd(count, 1);
    // remarque : peut mieux faire, utiliser une hierarchie de compteurs atomiques, 1 par sous groupe, 1 par groupe, 1 global
    // cf le principe et la solution AMD https://gpuopen.com/fast-compaction-with-mbcnt/
    // cf equivalent nvidia https://developer.nvidia.com/reading-between-threads-shader-intrinsics
    
    // etape 2 : initialiser les parametres
    params[index].vertex_count= objects[id].vertex_count;
    params[index].instance_count= 1;
    params[index].vertex_base= objects[id].vertex_base;
    params[index].instance_base= 0;
    
    // etape 3 : conserve aussi l'indice de l'objet...
    remap[index]= id;
}

#endif
