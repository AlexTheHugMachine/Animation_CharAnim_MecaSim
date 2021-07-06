
//! \file decal.glsl

#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 2) in vec3 normal;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat4 decalMatrix;

out vec4 decal_position;

out vec3 vertex_position;
out vec3 vertex_normal;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    decal_position= decalMatrix * vec4(position, 1);
    
    vertex_position= vec3(mvMatrix * vec4(position, 1));
    vertex_normal= mat3(mvMatrix) * normal;
}
#endif

#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

uniform sampler2D decal;
in vec4 decal_position;

in vec3 vertex_position;
in vec3 vertex_normal;

void main( )
{
    vec3 l= normalize(-vertex_position);        // la camera est la source de lumiere.
    vec3 n= normalize(vertex_normal);
    float cos_theta= max(0, dot(n, l));
    vec3 color= vec3(0.8) * cos_theta;
    
    //~ vec3 texcoord= decal_position.xyz / decal_position.w;
    //~ vec3 decal_color= texture(decal, texcoord.xy).rgb;
    //~ if(texcoord.x < 0 || texcoord.x > 1)
        //~ color.g= 1;
    //~ if(texcoord.y < 0 || texcoord.y > 1)
        //~ color.r= 1;
    //~ if(texcoord.z < 0 || texcoord.z > 1)
        //~ color.b= 1;
		
    //~ if((texcoord.x < 0 || texcoord.x > 1)
    //~ || (texcoord.y < 0 || texcoord.y > 1)
    //~ || (texcoord.z < 0 || texcoord.z > 1))
        //~ decal_color= vec3(1);
		
    //~ float decal_z= decal_position.z / decal_position.w;
	//~ if(decal_z < 0 || decal_z > 1)
		//~ color.r= 1;
	//~ else
		//~ color= color * textureProj(decal, decal_position).rgb;

	vec3 decal_color= textureProj(decal, decal_position).rgb;
    
    fragment_color= vec4(color * decal_color, 1);
}
#endif
