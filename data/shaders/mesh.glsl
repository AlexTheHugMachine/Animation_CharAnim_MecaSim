#version 330

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 position;
out vec3 vertex_position;

#ifdef USE_TEXCOORD
    layout(location= 1) in vec2 texcoord;
    out vec2 vertex_texcoord;
#endif

#ifdef USE_NORMAL
    layout(location= 2) in vec3 normal;
    out vec3 vertex_normal;
#endif

#ifdef USE_COLOR
    layout(location= 3) in vec3 color;
    out vec3 vertex_color;
#endif

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat4 normalMatrix;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_position= vec3(mvMatrix * vec4(position, 1));
    
#ifdef USE_TEXCOORD
    vertex_texcoord= texcoord;
#endif
    
#ifdef USE_NORMAL
    vertex_normal= mat3(normalMatrix) * normal;
#endif
    
#ifdef USE_COLOR
    vertex_color= color;
#endif
}
#endif


#ifdef FRAGMENT_SHADER

in vec3 vertex_position;

#ifdef USE_TEXCOORD
    in vec2 vertex_texcoord;
    uniform sampler2D diffuse_color;
#endif

#ifdef USE_NORMAL
    in vec3 vertex_normal;
#endif

#ifdef USE_COLOR
    in vec3 vertex_color;
#endif

out vec4 fragment_color;

void main( )
{
    vec3 color= vec3(1, 1, 1);  // prevoir un uniform ?
#ifdef USE_COLOR
    color= vertex_color;
#endif

#ifdef USE_TEXCOORD
    color= color * texture(diffuse_color, vertex_texcoord).rgb;
#endif
    
    vec3 normal;
#ifdef USE_NORMAL
    normal= vertex_normal;
#else
    vec3 t= normalize(dFdx(vertex_position));
    vec3 b= normalize(dFdy(vertex_position));
    normal= normalize(cross(t, b));
#endif
    
    color= color * normal.z;
    // hachure les triangles mal orientes
    if(!gl_FrontFacing)
    {
        ivec2 pixel= ivec2(gl_FragCoord.xy) % ivec2(4, 4);
        if((pixel.x ^ pixel.y) == 0)
            color= vec3(0.8, 0.4, 0);
        //~ else
            //~ color= vec3(0, 0, 0);
    }
    
    fragment_color= vec4(color, 1);
}
#endif
