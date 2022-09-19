
#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoord;
layout(location= 2) in vec3 normal;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

out vec3 vertex_position;
out vec3 vertex_normal;
out vec2 vertex_texcoord;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    
    // position et normale dans le repere camera
    vertex_position= vec3(mvMatrix * vec4(position, 1));
    vertex_normal= mat3(mvMatrix) * normal;
    vertex_texcoord= texcoord;
}

#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_texcoord;

uniform vec4 material_color;
uniform float material_metallic;
uniform float material_roughness;
uniform vec4 material_emission;

uniform sampler2D material_color_texture;
uniform sampler2D material_metallic_roughness_texture;
uniform sampler2D material_occlusion_texture;
uniform sampler2D material_normal_texture;
uniform sampler2D material_emission_texture;


// gltf spec 
const float PI= 3.141592;

float D( const in float alpha, const in float NdotH )
{
	if(NdotH < 0) return 0.0;
	
	float a2= alpha*alpha;
	float d= NdotH*NdotH * (a2 -1) + 1;
	return a2 / (PI * d*d);
}

float GV( const in float alpha, const in float HdotL, const in float HdotV, const in float NdotL, const in float NdotV )
{
	if(HdotL < 0) return 0.0;
	if(HdotV < 0) return 0.0;
	
	float a2= alpha*alpha;
	float nl= abs(NdotL)+ sqrt(a2 + (1 - a2)*NdotL*NdotL);
	float nv= abs(NdotV) + sqrt(a2 + (1 - a2)*NdotV*NdotV);
	return 1 / nl + 1 / nv;
}


void main( )
{
// geometrie
	vec3 N= normalize(vertex_normal);
	//~ vec3 N= cross(normalize(dFdx(vertex_position)), normalize(dFdy(vertex_position)));			// n, normale au point p

#if 1
// cf "Surface Gradient-Based Bump Mapping Framework", mikkelsen, jcgt 2020
// https://jcgt.org/published/0009/03/04/paper.pdf

// normal map
	vec3 M= 2 * texture(material_normal_texture, vertex_texcoord).xyz - vec3(1);

	// listing 2, tbn normal map to derivative
	//~ float flip_deriv= -1;
	float flip_deriv= 1;
	vec2 dM;
	{
		float scale= 1.0 / 128.0;
		vec3 v= abs(M);
		float z= max(v.z, scale * max(v.x, v.y));
		dM= vec2(-v.x, -flip_deriv * v.y) / z;
	}
	
// repere tangent
	// listing 4, procedural tbn basis
	vec3 T;
	vec3 B;
	{
		vec3 dPdx= dFdx(vertex_position);
		vec3 dPdy= dFdy(vertex_position);
		
		vec2 dSTdx= dFdx(vertex_texcoord);
		vec2 dSTdy= dFdy(vertex_texcoord);
		float det= dot(dSTdx, vec2(dSTdy.y, -dSTdy.x));
		float sign_det= det < 0.0 ? -1.0 : 1.0;
		
		vec2 invC0= sign_det * vec2(dSTdy.y, -dSTdx.y);
		T= dPdx*invC0.x + dPdy*invC0.y;
		if(abs(det) > 0) T= normalize(T);
		B= sign_det * cross(N, T);
	}
	
// normale perturbee
	// listing 3, tbn style surface gradient
	M= dM.x*T + dM.y*B;
	N= normalize(N - M);
#endif

  	vec3 V= normalize(-vertex_position);
	//~ vec3 L= normalize(vec3(1,2,0) - vertex_position);	// la camera est aussi la source...
	vec3 L= normalize( -vertex_position);	// la camera est aussi la source...
	vec3 H= normalize(V+L);

// parametres
	vec3 color= material_color.rgb * texture(material_color_texture, vertex_texcoord).rgb;
	color= pow(color, vec3(2.2));
	
	float alpha= material_roughness * texture(material_metallic_roughness_texture, vertex_texcoord).g;
	alpha= max(0.01, alpha * alpha);
	float metallic= material_metallic * texture(material_metallic_roughness_texture, vertex_texcoord).b;
	
	vec3 black = vec3(0);
	vec3 diffuse= mix(color.rgb, black, metallic);
	vec3 F0= mix(vec3(0.04), color.rgb, metallic);

// diffuse
	float VdotH= min(1.0, max(0, dot(V, H)));
	vec3 F = F0 + (vec3(1) - F0) * pow((1 - abs(VdotH)), 5);
	vec3 fr_diffuse = (vec3(1) - F) * diffuse; // / PI;

// reflets 
	float NdotH= abs(dot(N, H));
	float NdotL= abs(dot(N, L));
	float NdotV= abs(dot(N, V));
	float HdotL= abs(dot(H, L));
	float HdotV= abs(dot(H, V));
	
	vec3 fr_specular = F * D(alpha, NdotH) * GV(alpha, HdotL, HdotV, NdotL, NdotV);
	//~ float ns= 6 * (2 / alpha - 1);
	//~ vec3 fr_specular = F / NdotL * (ns+8)/(8*PI) * pow(NdotH, ns);
	
// AO
	//~ float ao= texture(material_occlusion_texture, vertex_texcoord).r;
	float ao= texture(material_metallic_roughness_texture, vertex_texcoord).r;
	//~ ao= pow(ao, 2.2);
	//~ float ao= 1;
	
// emission
	vec3 Le= material_emission.rgb *  texture(material_emission_texture, vertex_texcoord).rgb;
	Le= pow(Le, vec3(2.2));
	//~ vec3 Le= vec3(0);

// brdf
	vec3 fr= fr_diffuse + fr_specular;
	//~ vec3 fr= fr_specular;

// evaluation
    fragment_color= vec4((Le + ao * fr) * NdotL, 1);
    //~ fragment_color= vec4(vec3(NdotV), 1);
    //~ fragment_color= vec4(abs(N), 1);
}

#endif
