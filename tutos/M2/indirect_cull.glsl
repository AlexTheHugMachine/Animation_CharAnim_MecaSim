
#version 430

#ifdef COMPUTE_SHADER

layout(binding= 0, std430) readonly buffer objectData
{
    struct
    {
        vec3 pmin;
        uint vertex_count;
        vec3 pmax;
        uint vertex_base;
    } objects[];
};

layout(binding= 1, std430) writeonly buffer paramData
{
    struct 
    {
        uint vertex_count;
        uint instance_count;
        uint vertex_base;
        uint instance_base;
    } params[];
};

layout(binding= 2) buffer counterData
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
    
    vec3 pmin= objects[id].pmin;
    vec3 pmax= objects[id].pmax;
    
    if(any(lessThan(pmax, bmin))        // trop a gauche pour x, etc
    || any(greaterThan(pmin, bmax)))    // trop a droite pour x, etc.
        // pas d'intersection...
        return;
    
    // emettre les parametres du draw
    // etape 1 : position dans le buffer de sortie
    uint index= atomicAdd(count, 1);
    
    // etape 2 : initialiser les parametres
    params[index].vertex_count= objects[id].vertex_count;
    params[index].instance_count= 1;
    params[index].vertex_base= objects[id].vertex_base;
    params[index].instance_base= 0;
}

#endif
