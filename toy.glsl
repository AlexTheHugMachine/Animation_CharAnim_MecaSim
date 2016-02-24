
#version 330

#ifdef VERTEX_SHADER

out vec2 position;

void main( )
{
    vec2 quad[4]= vec2[4]( vec2(-1,-1), vec2(1,-1), vec2(-1, 1), vec2(1, 1) );
    
    position= quad[gl_VertexID];
    gl_Position= vec4(quad[gl_VertexID], 0, 1);
}

#endif

#ifdef FRAGMENT_SHADER

float sphere( const vec3 p, const float radius )
{
    return length(p) - radius;
}

float displace( const vec3 p )
{
    return sin(30 * p.x) * sin(30 * p.y) * sin(30 * p.z) * 0.04;
}

float object( const vec3 p )
{
    float d1= sphere(p, 0.75);
    float d2= displace(p);
    //~ float d2= 0;
    
    return d1+d2;
}


uniform mat4 mvpInvMatrix;
uniform mat4 mvMatrix;

in vec2 position;

out vec4 fragment_color;

void main( )
{
    // construction du rayon pour le pixel
    vec4 oh= mvpInvMatrix * vec4(position, 0, 1);  // origine sur near
    vec4 eh= mvpInvMatrix * vec4(position, 1, 1);  // extremite sur far
    
    // origine et direction normalisee
    vec3 o= oh.xyz / oh.w;                         // origine
    vec3 d= eh.xyz / eh.w - oh.xyz / oh.w;         // direction
    d= normalize(d);
    
    float t= 0.0;
    float distance= 0.0;
    vec3 p;
    for(int i= 0; i < 128; i++)
    {
        p= o + t * d;
        
        distance= object(p);
        t= t + distance / 1.5;
    }
    
    vec3 n= cross(normalize(dFdx(p)), normalize(dFdy(p)));
    fragment_color= vec4(abs(n), 1);
}

#endif
