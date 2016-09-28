#version 330

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 position;

layout(location= 2) in vec3 normal;
out vec3 vertex_normal;

uniform mat4 mvpMatrix;
uniform mat4 normalMatrix;

void main( )
{
    // retrouve la position dans la grille 5x5 en fonction de l'identifiant de la copie / instance
    float x= float(gl_InstanceID % 5) - 2;
    float y= float(gl_InstanceID / 5) - 2;
    
    // deplace le sommet pour le placer dans la grille
    gl_Position= mvpMatrix * vec4(position + vec3(x * 20, y * 20, 0), 1);
    vertex_normal= mat3(normalMatrix) * normal;
}
#endif


#ifdef FRAGMENT_SHADER

in vec3 vertex_normal;

out vec4 fragment_color;

void main( )
{
    vec3 normal= normalize(vertex_normal);
    float cos_theta= abs(normal.z);
    //~ vec3 color= vec3(1, 1, 1);
    vec3 color= abs(normal);
    color= color * cos_theta;
    
    fragment_color= vec4(color, 1);
}
#endif
