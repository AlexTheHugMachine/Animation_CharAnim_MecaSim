
//! \file cubemap.glsl dessiner la cubemap a l'infini.

#version 330

#ifdef VERTEX_SHADER
uniform mat4 vpInvMatrix;

out vec3 vertex_position;

void main( )
{
    // repere projectif
    vec2 positions[3]= vec2[3]( vec2(-1,-1), vec2(3, -1), vec2(-1, 3) );
    
    // place le point sur le plan far... a l'infini
    vec4 p= vec4(positions[gl_VertexID], 1, 1);

    // repere monde
    vec4 ph= vpInvMatrix * p;
    vertex_position= ph.xyz / ph.w;
    
    gl_Position= p;
}
#endif

#ifdef FRAGMENT_SHADER
uniform vec3 camera_position;
uniform samplerCube texture0;

in vec3 vertex_position;
out vec4 fragment_color;

void main( )
{
    // calculer la direction du rayon pour le pixel dans le repere du monde
    vec3 d= normalize(vertex_position - camera_position);
    vec4 color= texture(texture0, d);
    
    fragment_color= color;
}
#endif
