
#version 430

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoords;

out vec3 vertex_position;
out vec2 vertex_texcoords;
out vec3 view_position;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_position= position;
    vertex_texcoords= texcoords;
    view_position= vec3(mvMatrix * vec4(position, 1));
    
}
#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;
in vec3 vertex_position;
in vec2 vertex_texcoords;
in vec3 view_position;

uniform sampler2D alpha;

float hash2D( in vec2 v ) { return fract(1.0e4*sin(17.0*v.x + 0.1*v.y) * (0.1 + abs(sin(13.0*v.y + v.x)))); }
float hash3D( in vec3 v ) { return hash2D(vec2(hash2D(v.xy), v.z)); }

const int n= 8; // MSAA samples

void main()
{
// enable GL_MULTISAMPLE
// enable GL_SAMPLE_ALPHA_TO_COVERAGE

    //~ // alpha test base
    //~ if(0.5 < rng)
        //~ discard;
    
    // hashed alpha test 
    //~ float rng= hash3D(vertex_position);
    //~ fragment_color= vec4(0.9, 0.4, 0, rng);

// construit le masque des samples 
    vec4 color= texture(alpha, vertex_texcoords);
    //~ vec4 color= vec4(0.8, 0.8, 0.9, 0.15);
    // threshold
    int i= int(floor(color.a * n));
    // jitter threshold
    float rng= (i+ hash3D(vertex_position)) / float(n);
    //~ // no jitter
    //~ float rng= i / float(n);
    
    fragment_color= vec4(color.rgb, rng);
}

#endif
