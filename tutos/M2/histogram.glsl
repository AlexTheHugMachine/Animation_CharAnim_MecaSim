
// version finale

#version 430

#ifdef COMPUTE_SHADER

layout(std430, binding= 0) coherent buffer HistogramData
{
    int histogram[16];
};

// image resultat
layout(binding= 0, rgba8)  readonly uniform image2D image;

shared int group_histogram[16];

layout(local_size_x= 8, local_size_y= 8) in;
void main( )
{
    uint ID= gl_LocalInvocationIndex;
    if(ID < 16)
        group_histogram[ID]= 0;
    
    // attend que tous les sous groupes s'executent
    barrier();

    int local_histogram[16]= int[16](0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    for(int i= 0; i < 16; i++)
    {
        ivec2 offset= ivec2((i % 4), (i / 4));
        vec4 pixel= imageLoad(image, ivec2(gl_WorkGroupID.xy)*32 + ivec2(gl_LocalInvocationID.xy) + offset*8);
        
        // calculer la cellule de l'histogramme pour le pixel
        float grey= (pixel.r + pixel.g + pixel.b) / 3;      // entre 0 et 1
        int bin= int(grey * 15);
        
        local_histogram[bin]++;
    }

    for(int i= 0; i < 16; i++)
        if(local_histogram[i] > 0)
            atomicAdd(group_histogram[i], local_histogram[i]);
    
    // attend que tous les sous groupes s'executent
    barrier();
    
    if(ID < 16)
        atomicAdd(histogram[ID], group_histogram[ID]);
}

#endif
