
#version 330

#ifdef VERTEX_SHADER
out vec2 vertex_texcoord;

void main(void)
{
    vec3 positions[3]= vec3[3]( vec3(-1, 1, -1), vec3( -1, -3, -1), vec3( 3,  1, -1) );
    
    gl_Position= vec4(positions[gl_VertexID], 1.0);
    vertex_texcoord= positions[gl_VertexID].xy / 2.0 + 0.5;
}

#endif


#ifdef FRAGMENT_SHADER
uniform float compression= 1;
uniform float saturation= 1;
uniform vec4 channels= vec4(1, 1, 1, 1);
uniform float gray= 0;

uniform sampler2D image;

in vec2 vertex_texcoord;
out vec4 fragment_color;

void main(void)
{
    const vec3 rgby= vec3(0.3, 0.59, 0.11);
    float k1= 1.0 / pow(saturation, 1.0 / compression); // normalisation : saturation == blanc
    
    vec4 color= texture(image, vertex_texcoord);
    
    if(any(isnan(color)))
    {
        fragment_color= vec4(1, 0, 1, 1);
        return;
    }
    
    float y= dot(color.rgb, rgby);  // normalisation de la couleur : (color / y) == teinte
    //~ if(y > saturation)
        //~ color.rgb= vec3(y, y, y);
    
    // applique la compression (gamma)
    color= (color / y) * k1 * pow(y, 1.0 / compression);

    if(channels != vec4(0, 0, 0, 1))
    {
        color.rgb= channels.rgb * color.rgb;
        if(gray > 0.f)
        {
            float g= max(color.r, max(color.g, color.b));
            color.rgb= vec3(g, g, g);
        }
    }
    else
        // visualisation du canal alpha seul
        color.rgb= color.aaa;
    
    fragment_color= vec4(clamp(color.rgb, vec3(0.0), vec3(1.0)), 1.0);
}

#endif
