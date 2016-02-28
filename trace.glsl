
#version 330

#ifdef VERTEX_SHADER

out vec2 position;

void main( )
{
    vec2 positions[3]= vec2[3]( vec2(-1,-3), vec2(3, 1), vec2(-1, 1) );
    
    position= positions[gl_VertexID];
    gl_Position= vec4(positions[gl_VertexID], 0, 1);
}

#endif

#ifdef FRAGMENT_SHADER

/*
catalogue de fonctions sur les spheres :
http://www.iquilezles.org/www/articles/spherefunctions/spherefunctions.htm
*/

#define inf 999999.0

float sphere( const in vec3 o, const in vec3 d, const in vec3 center, const in float radius )
{
    vec3 oc= o - center;
    float b= dot(oc, d);

    float c= dot(oc, oc) - radius*radius;
    float h= b*b - dot(d, d) * c;
    if(h < 0.0) return inf;
    h = sqrt(h);
    return (-b - h) / dot(d, d);
    
    // on peut simplifier si la direction du rayon est normalisee, dot(d, d) == 1...
}

float plane( const in vec3 o, const in vec3 d, const in vec3 anchor, const in vec3 normal )
{
    float t= dot(anchor - o, normal) / dot(d, normal);
    if(t < 0.0) return inf;
    return t;
}


float object( const in vec3 o, const in vec3 d )
{
    float t1= sphere(o, d, vec3(0, 0, 0), 0.5);
    float t2= plane(o, d, vec3(0, -1, 0), vec3(0, 1, 0));
    
    return min(t1, t2);
}

uniform mat4 mvpInvMatrix;
uniform mat4 mvMatrix;

in vec2 position;

out vec4 fragment_color;

void main( )
{
    // construction du rayon pour le pixel
    vec4 oh= mvpInvMatrix * vec4(position, 0, 1);       // origine sur near
    vec4 eh= mvpInvMatrix * vec4(position, 1, 1);       // extremite sur far
    
    // origine et direction
    vec3 o= oh.xyz / oh.w;                              // origine
    vec3 d= eh.xyz / eh.w - oh.xyz / oh.w;              // direction
    
    float t= object(o, d);
    vec3 p= o + t*d;
    // calculer correctement la normale de l'intersection...
    vec3 n= normalize(cross(
        normalize(dFdx( vec3(mvMatrix * vec4(p, 1)) )), 
        normalize(dFdy( vec3(mvMatrix * vec4(p, 1)) ))
    ));
    
    float shadow= object(p + n * 0.01, vec3(0, 1, 0));
    if(shadow < 1.0)
        fragment_color= vec4(0, 0, 0, 1);
    else
        fragment_color= vec4(abs(n), 1);
}

#endif
