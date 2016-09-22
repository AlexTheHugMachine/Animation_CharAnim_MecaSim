
#version 330

#ifdef VERTEX_SHADER
in vec3 position;
in vec2 texcoord;

uniform mat4 mvpMatrix;

out vec2 vertex_texcoord;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_texcoord= texcoord;
}
#endif


#ifdef FRAGMENT_SHADER

in vec2 vertex_texcoord;

uniform sampler2D base_texture;
uniform sampler2D detail_texture;

out vec4 fragment_color;

void main( )
{
    vec4 color= texture(base_texture, vertex_texcoord);
    color= color * 0.5 + texture(detail_texture, vertex_texcoord) * 0.5;
    
    fragment_color= color;
}
#endif
