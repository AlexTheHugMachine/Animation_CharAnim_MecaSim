
#version 330

#ifdef VERTEX_SHADER

const float dx= 0.0;
const float dy= 0;
const float dz= 0;

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

    // calculer le resultat 
    vec4 r;
    r.x= p.x + dx;
    r.y= p.y + dy;
    r.z= p.z + dz;
    r.w= 1;
    
    // renvoyer le sommet transforme
    gl_Position= r;
}
#endif


#ifdef FRAGMENT_SHADER

out vec4 fragment_color;

void main( )
{
    // remplir le triangle avec une couleur uniforme
    framgent_color= vec4(0.8, 0.4, 0, 1);
}
#endif
