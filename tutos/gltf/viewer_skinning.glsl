
#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoord;
layout(location= 2) in vec3 normal;
layout(location= 3) in uvec4 joints;
layout(location= 4) in vec4 weights;

uniform mat4 matrices[64];

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

out vec3 vertex_position;
out vec3 vertex_normal;
out vec2 vertex_texcoord;

void main( )
{
    mat4 mx= matrices[joints.x]; 
    mat4 my= matrices[joints.y]; 
    mat4 mz= matrices[joints.z]; 
    mat4 mw= matrices[joints.w];
    mat4 m= weights.x * mx + weights.y * my + weights.z * mz + weights.w * mw;
    
    gl_Position= mvpMatrix * m * vec4(position, 1);
    
    // position et normale dans le repere camera
    vertex_position= vec3(mvMatrix * m * vec4(position, 1));
    vertex_normal= mat3(mvMatrix) * mat3(m) * normal;
    vertex_texcoord= texcoord;
}

#endif


#if 0
out vec4 fragment_color;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_texcoord;
in vec4 vertex_weights;

uniform vec4 material_color;

void main( )
{
    vec3 l= normalize(-vertex_position);        // l, direction vers la lumiere // la camera est la source de lumiere. (0,0,0) dans le repere camera
    //~ vec3 n= normalize(vertex_normal);			// n, normale au point p
    vec3 n= cross(normalize(dFdx(vertex_position)), normalize(dFdy(vertex_position)));			// n, normale au point p
    float cos_theta= max(0, dot(n, l));			// cos theta, angle entre la normale et la direction vers la lumiere
    
    //~ fragment_color= vec4(1, 0.5, 0.5, 1) * material_color * cos_theta;	// modele de lambert
    fragment_color= material_color * cos_theta;	// modele de lambert
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
uniform sampler2D material_color_texture;
uniform sampler2D material_metallic_roughness_texture;
uniform sampler2D material_occlusion_texture;

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
  
	vec3 V= normalize(-vertex_position);
	vec3 L= normalize(-vertex_position);	// la camera est aussi la source...
	vec3 H= normalize(V+L);

// parametres
	vec3 color= material_color.rgb * texture(material_color_texture, vertex_texcoord).rgb;
	color= pow(color, vec3(2.2));

	float alpha= material_roughness * texture(material_metallic_roughness_texture, vertex_texcoord).g;
	alpha= alpha * alpha;
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
	
// AO
	float ao= texture(material_occlusion_texture, vertex_texcoord).r;
	
// brdf
	vec3 fr= fr_diffuse + fr_specular;

// evaluation
    fragment_color= vec4(ao * fr * NdotL, 1);
}

#endif

