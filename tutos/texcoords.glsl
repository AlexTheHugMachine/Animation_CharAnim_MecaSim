
#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoord;
layout(location= 2) in vec3 normal;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat4 normalMatrix;

out vec3 vertex_position;
out vec2 vertex_texcoord;
out vec3 vertex_normal;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_position= vec3(mvMatrix * vec4(position, 1));
    vertex_texcoord= texcoord;
    vertex_normal= mat3(normalMatrix) * normal;   // uniquement une rotation, mat3 suffit 
    
}
#endif

#ifdef FRAGMENT_SHADER
uniform sampler2D color_texture;

in vec3 vertex_position;
in vec2 vertex_texcoord;
in vec3 vertex_normal;

out vec4 fragment_color;

void main( )
{
    // place la source de lumiere sur la camera, repere camera, position 0, 0, 0
    float cos_theta= dot( normalize(vertex_normal), normalize(-vertex_position));
    //~ vec4 color= textureLod(color_texture, vertex_texcoord, 0);
    vec4 color= texture(color_texture, vertex_texcoord);
    fragment_color= cos_theta * color;
}
#endif
