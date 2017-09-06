
//! \file tuto1GL_vertex.glsl

#version 330

/*  vec3 et vec4 sont des types de base, ils sont equivalents a :
    
    struct vec3 { float x, y, z; };
    struct vec4 { float x, y, z, w; };
 */

void main( )
{
    // intialiser les coordonnees des 3 sommets
    vec3 positions[3]= vec3[3]( vec3(-0.5, -0.5, 0), vec3(0.5, -0.5, 0), vec3(0, 0.5, 0) );
    
    // recuperer le sommet a traiter
    vec3 p= positions[gl_VertexID];
    
    // renvoyer le resultat du vertex shader, positon du sommet dans le repere projectif
    gl_Position= vec4(p, 1);
}
