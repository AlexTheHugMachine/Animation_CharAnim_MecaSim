#version 330


#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 2) in vec3 normal;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;

out vec3 p;
out vec3 n;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    
    p= vec3(modelMatrix * vec4(position, 1));
    n= mat3(modelMatrix) * normal;
}
#endif

#ifdef FRAGMENT_SHADER
in vec3 p;
in vec3 n;

uniform mat4 viewInvMatrix;

//~ const vec3 source= vec3(0, 0, 0);   // source dans le repere du monde
const vec3 emission= vec3(1);
const float k= 0.8;

const float PI= 3.14159265359;

out vec4 fragment_color;
void main( )
{
    vec3 camera= vec3(viewInvMatrix * vec4(0, 0, 0, 1));        // position de la camera dans le repere du monde
    vec3 source= vec3(viewInvMatrix * vec4(10, 10, 0, 1));        // source "frontale" positionnee par rapport a la camera
    
    // directions 
    vec3 o= normalize(camera - p);
    vec3 l= normalize(source - p);
    // cos
    float cos_theta= max(0, dot(normalize(n), l));
    
    // brdf
    float fr= k / PI;
    vec3 color= emission * fr * cos_theta;
    
    fragment_color= vec4(color, 1);
}
#endif
