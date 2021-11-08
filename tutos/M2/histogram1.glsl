
// version directe, pas tres rapide...

#version 430

#ifdef COMPUTE_SHADER

layout(std430, binding= 0) coherent buffer HistogramData
{
    int histogram[16];
};

// image resultat
layout(binding= 0, rgba8)  readonly uniform image2D image;

layout(local_size_x= 8, local_size_y= 8) in;
void main( )
{
    vec4 pixel= imageLoad(image, ivec2(gl_GlobalInvocationID.xy));
    
    // calculer la cellule de l'histogramme pour le pixel
    float grey= (pixel.r + pixel.g + pixel.b) / 3;      // entre 0 et 1
    int bin= int(grey * 15);
    
    atomicAdd(histogram[bin], 1);
}

#endif
