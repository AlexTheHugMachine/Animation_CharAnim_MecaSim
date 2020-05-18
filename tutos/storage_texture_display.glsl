
//! \file storage_texture_display.glsl

#version 430

#ifdef VERTEX_SHADER

void main( )
{
    vec3 positions[3]= vec3[3]( vec3(-1, 1, -1), vec3( -1, -3, -1), vec3( 3,  1, -1) );

    gl_Position= vec4(positions[gl_VertexID], 1.0);
}
#endif


#ifdef FRAGMENT_SHADER

layout(binding= 0, r32ui) readonly uniform uimage2D image;

out vec4 fragment_color;

void main( )
{
    // utilise une palette de couleur pour afficher la valeur du compteur
    const vec3 colors[10]= vec3[10](
        vec3(0,0,0), 
        vec3(12,17,115),
        vec3(28,121,255),
        vec3(31,255,255),
        vec3(130,255,17),
        vec3(255,255,14),
        vec3(255,112,22),
        vec3(251,0,20),
        vec3(113,1,14),
        vec3(113,1,14)
    );
    
    uint n= imageLoad(image, ivec2(gl_FragCoord.xy)).r;
    vec3 color;
    if(n < 10) color= colors[n] / vec3(255);
    else color= colors[9] / vec3(255);
    
    fragment_color= vec4(color, 1.0);
}
#endif
