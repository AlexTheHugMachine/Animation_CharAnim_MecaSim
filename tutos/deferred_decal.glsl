
#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;

uniform mat4 mvpMatrix;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
}
#endif

#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

uniform mat4 invMatrix;

uniform sampler2D decal_texture;
uniform sampler2D color_texture;
uniform sampler2D depth_texture;

void main( )
{
    // couleur de l'objet deja dessine, pour la modifier en fonction du decal
    vec3 color= texelFetch(color_texture, ivec2(gl_FragCoord.xy), 0).rgb;
    
    // retrouve la position du fragment deja dessine dans le monde 
    float z= texelFetch(depth_texture, ivec2(gl_FragCoord.xy), 0).r;
    vec4 position= vec4(gl_FragCoord.xy, z, 1);
    
    // passage dans le repere du decal
    vec4 decal_position= invMatrix * position;

    vec3 decal_color= textureProj(decal_texture, decal_position).rgb;
    
    // test d'inclusion
    vec3 texcoord= decal_position.xyz / decal_position.w;
    //~ if(texcoord.x < 0 || texcoord.x > decal_position.w)
    if(texcoord.x < 0 || texcoord.x > 1)
        discard;
        //~ color.g= 1;
        //~ color= vec3(1);
    //~ if(texcoord.y < 0 || texcoord.y > decal_position.w)
    if(texcoord.y < 0 || texcoord.y > 1)
        discard;
        //~ color.r= 1;
        //~ color= vec3(1);
	//~ if(texcoord.z < 0 || texcoord.z > decal_position.w)
	if(texcoord.z < 0 || texcoord.z > 1)
        discard;
		//~ color.b= 1;
        //~ color= vec3(1);
    
    color= color * decal_color;
    fragment_color= vec4(color, 1);
}
#endif
