
#version 330

#ifdef VERTEX_SHADER

out vec3 position;

const float dx= 0.0;
const float dy= 0;
const float dz= 0;

void main( )
{
    vec3 positions[3]= vec3[3]( vec3(-0.5, -0.5, 0), vec3(0.5, -0.5, 0), vec3(0, 0.5, 0) );
    vec3 p= positions[gl_VertexID];

    vec4 r;
    r.x= p.x + dx;
    r.y= p.y + dy;
    r.z= p.z + dz;
    r.w= 1;
    
    gl_Position= r;
}
#endif


#ifdef FRAGMENT_SHADER

out vec4 fragment_color;

void main( )
{
    fragment_color= vec4(0.8, 0.4, 0, 1);
}
#endif
