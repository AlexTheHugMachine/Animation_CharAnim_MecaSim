#version 100

precision highp float ;
precision highp int ;

#ifdef VERTEX_SHADER
attribute vec3 position;

#ifdef USE_COLOR
    attribute vec4 color;
    varying vec4 vertex_color;
#endif

uniform mat4 mvpMatrix;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1.0);
#ifdef USE_COLOR
    vertex_color= color;
#endif
}
#endif

#ifdef FRAGMENT_SHADER

#ifdef USE_COLOR
    in vec4 vertex_color;
#endif

uniform vec4 mesh_color;

void main( )
{
    vec4 color= mesh_color;
#ifdef USE_COLOR
    color= vertex_color;
#endif
    
    gl_FragColor= color;
}
#endif
