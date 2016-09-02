
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

uniform float time;
uniform vec3 motion;    // x, y, button > 0
uniform vec3 mouse;     // x, y, button > 0
uniform vec2 viewport;

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
    
    h= sqrt(h);
    float t1= (-b - h);
    
/*  solution classique avec les 2 racines :
 */
    float t2= (-b + h);
    if(t1 <= 0 && t2 <= 0)
        return inf;
    else if(t1 > 0 && t2 > 0)
        return min(t1, t2) / dot(d, d); // plus petite racine postive
    else
        return max(t1, t2) / dot(d, d); // racine positive

/*  solution courte : si l'origine du rayon est a l'exterieur de la sphere... 
    if(t1 < 0)
        return inf;
    else
        return t1 / dot(d, d);
 */

    // on peut simplifier si la direction du rayon est normalisee, dot(d, d) == 1...
}

float plane( const in vec3 o, const in vec3 d, const in vec3 anchor, const in vec3 normal )
{
    float t= dot(anchor - o, normal) / dot(d, normal);
    if(t < 0.0) return inf;
    return t;
}


bool object( const in vec3 o, const in vec3 d, out float t, out vec3 n )
{
    vec3 center= vec3(0, 0, 0) + vec3(cos(time / 1000), sin(time / 4000) /2, sin(time / 1000));
    float radius= 0.6;
    float t1= sphere(o, d, center, radius);
    float t2= plane(o, d, vec3(0, -0.5, 0), vec3(0, 1, 0));
    
    if(t1 < inf && t1 < t2)
    {
        n= normalize(o + t1*d - center); 
        t= t1;
        return true;
    }
    else if(t2 < inf && t2 < t1)
    {
        n= vec3(0, 1, 0);
        t= t2;
        return true;
    }
    
    n= vec3(0, 0, 1);
    t= inf;
    return false;
}

uniform mat4 mvpInvMatrix;
uniform mat4 mvMatrix;

in vec2 position;

out vec4 fragment_color;

void main( )
{
    // construction du rayon pour le pixel, passage depuis le repere projectif
    vec4 oh= mvpInvMatrix * vec4(position, 0, 1);       // origine sur near
    vec4 eh= mvpInvMatrix * vec4(position, 1, 1);       // extremite sur far
    
    // origine et direction
    vec3 o= oh.xyz / oh.w;                              // origine
    vec3 d= eh.xyz / eh.w - oh.xyz / oh.w;              // direction
    d= normalize(d);
    
    vec3 n;
    float t;
    if(object(o, d, t, n))
    {
        vec3 p= o + t*d;
        vec3 light= normalize(vec3(2, 1, 0));
        vec3 shadown;
        float shadow;
        bool hit= object(p + n * 0.001, light, shadow, shadown);
        
        vec3 color= vec3(1, 0.9, 0.9) * abs(dot(n, light));
        if(hit) color= color*0.3;
        fragment_color= vec4(color, 1);
    }
    else
        // env map
        fragment_color= vec4(abs(normalize(d)), 1);
    
}
#endif
