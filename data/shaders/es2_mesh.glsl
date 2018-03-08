#version 100

#extension GL_OES_standard_derivatives : enable

precision highp float ;
precision highp int ;

#ifdef VERTEX_SHADER

attribute vec3 position;
uniform mat4 mvpMatrix;

uniform mat4 mvMatrix;
varying vec3 vertex_position;

#ifdef USE_TEXCOORD
    attribute vec2 texcoord;
    varying vec2 vertex_texcoord;
#endif

#ifdef USE_NORMAL
    attribute vec3 normal;
    uniform mat4 normalMatrix;
    varying vec3 vertex_normal;
#endif

#ifdef USE_COLOR
    attribute vec4 color;
    varying vec4 vertex_color;
#endif


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

#ifdef USE_TEXCOORD
    varying vec2 vertex_texcoord;
    uniform sampler2D diffuse_color;
#endif

#ifdef USE_ALPHATEST
    uniform float alpha_min= 0.3;
#endif

varying vec3 vertex_position;

#ifdef USE_NORMAL
    varying vec3 vertex_normal;
#endif

#ifdef USE_COLOR
    varying vec4 vertex_color;
#endif

#ifdef USE_LIGHT
uniform vec3 light;
uniform vec4 light_color;
#endif

uniform vec4 mesh_color;

void main( )
{
    vec4 color= mesh_color;
#ifdef USE_COLOR
    color= vertex_color;
#endif

#ifdef USE_TEXCOORD
    color= color * texture2D(diffuse_color, vertex_texcoord);
    
    #ifdef USE_ALPHATEST
        if(color.a < alpha_min)
        //~ if(length(color.rgb) < alpha_min)
            discard;
    #endif
    
    if(color.r + color.g + color.b == 0.0)    // noir
    {
        // fixer une couleur debug pour indiquer qu'il faut utiliser une texture avec cet objet
        vec3 p= vertex_position * 8.0;
	float d= length( p - (floor(p) + 0.5));
	if(d > 1.0) d= 0.0;
        color=  vec4(d*0.8*2.0, d*0.4*2.0, 0.0, 1.0);
    }
#endif

    vec3 normal= vec3(0.0, 0.0, 1.0);
#ifdef USE_NORMAL
    normal= normalize(vertex_normal);
#else
    vec3 t= normalize(dFdx(vertex_position));
    vec3 b= normalize(dFdy(vertex_position));
    normal= normalize(cross(t, b));
#endif

#ifndef USE_ALPHATEST
    float cos_theta= 1.0;
    #ifdef USE_LIGHT
        cos_theta= max(0.0, dot(normal, normalize(light - vertex_position)));         // eclairage, uniquement des faces bien orientees
        color= color * light_color;
    #else
        cos_theta= normal.z;
    #endif
    
    // hachure les triangles mal orientes
    if(gl_FrontFacing == false) // if(!gl_FrontFacing) bug sur mac ?!
    {
        ivec2 pixel= ivec2(floor(gl_FragCoord.xy / 4.0) - floor(gl_FragCoord.xy / 8.0) * 2.0);
        if(pixel.x + pixel.y != 1)
            color= vec4(0.8, 0.4, 0.0, 1.0);
    }
    
    color.rgb= color.rgb * cos_theta;
#endif
    
    gl_FragColor= color;
}
#endif
