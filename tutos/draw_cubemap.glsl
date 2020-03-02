
#version 330

#ifdef VERTEX_SHADER

void main( )
{
    // repere projectif
    vec2 positions[3]= vec2[3]( vec2(-1,-1), vec2(3, -1), vec2(-1, 3) );
    // place le point sur le plan far / z =1... a l'infini
    gl_Position= vec4(positions[gl_VertexID], 1, 1);
}
#endif

#ifdef FRAGMENT_SHADER

uniform mat4 invMatrix;
uniform vec3 camera;
uniform samplerCube texture0;

out vec4 fragment_color;

void main( )
{
    vec4 p= invMatrix * vec4(gl_FragCoord.xy, 1, 1);
    vec3 pixel= p.xyz / p.w;
    
    vec3 direction= normalize(pixel - camera);
    fragment_color= texture(texture0, direction);
    //~ fragment_color= vec4(abs(direction), 1);
}
#endif
