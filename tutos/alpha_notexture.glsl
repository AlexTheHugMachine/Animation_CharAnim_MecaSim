
#version 430

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoords;
layout(location= 2) in vec3 normal;

out vec3 vertex_position;
out vec2 vertex_texcoords;

out vec3 view_position;
out vec3 view_normal;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);

    vertex_position= position;
    vertex_texcoords= texcoords;
    
    view_position= vec3(mvMatrix * vec4(position, 1));
    view_normal= mat3(mvMatrix) * normal;
}
#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

in vec3 vertex_position;
in vec2 vertex_texcoords;

in vec3 view_position;
in vec3 view_normal;


float hash2D( in vec2 v ) { return fract(1.0e4*sin(17.0*v.x + 0.1*v.y) * (0.1 + abs(sin(13.0*v.y + v.x)))); }
float hash3D( in vec3 v ) { return hash2D(vec2(hash2D(v.xy), v.z)); }

const int msaa= 8; // MSAA samples

float fresnel( const in float F0, const in vec3 n, const in vec3 l )
{
    float ndotl= abs(dot(n, l));
    return F0 + (1 - F0) * pow(1 - ndotl, 5);
}

//~ layout(early_fragment_tests) in;

void main()
{
// enable GL_MULTISAMPLE
// enable GL_SAMPLE_ALPHA_TO_COVERAGE
	
    vec4 color= vec4(1);
	
    vec3 n= normalize(view_normal);
    vec3 o= normalize(- view_position);
    float f= fresnel(0.028, n, o);
	
    // threshold
    float i= floor(f * msaa);
    //~ // jitter threshold
    //~ float rng= min(msaa, (i + hash3D(view_position))) / float(msaa);
    //~ // no jitter
    //~ float rng= i / float(msaa);
	
    float rng= min(msaa, (f * msaa + hash3D(vec3(vertex_position.xy, view_position.z)) - 0.5)) / float(msaa);

    fragment_color= vec4(color.rgb, rng);
    //~ fragment_color= vec4(color.rgb, f);
    //~ fragment_color= vec4(vec3(f), 1);
}

#endif
