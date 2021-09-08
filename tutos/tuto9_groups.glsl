//! \file tuto9_groups.glsl

#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 2) in vec3 normal;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

out vec3 vertex_position;
out vec3 vertex_normal;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    
    // position et normale dans le repere camera
    vertex_position= vec3(mvMatrix * vec4(position, 1));
    vertex_normal= mat3(mvMatrix) * normal;
}

#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

in vec3 vertex_position;
in vec3 vertex_normal;

uniform vec4 material_color;

void main( )
{
    vec3 l= normalize(-vertex_position);        // l, direction vers la lumiere // la camera est la source de lumiere. (0,0,0) dans le repere camera
    vec3 n= normalize(vertex_normal);			// n, normale au point p
    float cos_theta= max(0, dot(n, l));			// cos theta, angle entre la normale et la direction vers la lumiere
    fragment_color= material_color * cos_theta;	// modele de lambert
}

#endif
