
//! \file tuto3GL.glsl affichage d'un cube, 12 triangles, 36 sommets
#version 330

#ifdef VERTEX_SHADER

uniform vec3 positions[36];

uniform float time;

const float dx= -0.5;
const float dy= -0.5;
const float dz= 0;

void main( )
{
    vec4 r;
    r.x= positions[gl_VertexID].x + dx;
    r.y= positions[gl_VertexID].y + dy;
    r.z= positions[gl_VertexID].z + dz;
    r.w= 1;
    
    gl_Position= r;
}
#endif


#ifdef FRAGMENT_SHADER

uniform vec4 color;

out vec4 fragment_color;

void main( )
{
    // remplir le triangle avec une couleur uniforme
    fragment_color= vec4(color.rgb, 1);
}
#endif
