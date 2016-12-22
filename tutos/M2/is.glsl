
#version 330

#ifdef VERTEX_SHADER
out vec2 texcoord;

void main(void)
{
    vec3 positions[3]= vec3[3]( vec3(-1, 1, -1), vec3( -1, -3, -1), vec3( 3,  1, -1) );
    
    gl_Position= vec4(positions[gl_VertexID], 1.0);
    texcoord= (positions[gl_VertexID].xy +1) / 2;
}

#endif

#ifdef FRAGMENT_SHADER
in vec2 texcoord;

uniform sampler2D ptexture;
uniform sampler2D ntexture;
uniform sampler2D vtexture;

uniform vec2 pixel;

out vec4 fragment_color;

void main( )
{
    vec3 q= texture(ptexture, texcoord).xyz;
    vec3 qn= texture(ntexture, texcoord).xyz;
    float v= texture(vtexture, texcoord).x;
    
    vec3 p= texture(ptexture, pixel).xyz;
    vec3 n= texture(ntexture, pixel).xyz;
    
    vec3 d= q - p;
    float cos_theta_p= max(0, dot(normalize(n), normalize(d)));
    float cos_theta_q= max(0, dot(normalize(qn), normalize(-d)));
    float dpq2= distance(p, q) * distance(p, q);
    
    vec3 color= vec3(0, 0, cos_theta_p * cos_theta_q / dpq2) + abs(vec3(cos_theta_p, cos_theta_p, cos_theta_p)) / 8;
    
    fragment_color= vec4(color, 1);
    
    if(distance(pixel, texcoord) < 0.01) 
        fragment_color= vec4(1, 1, 1, 1);
}
#endif

