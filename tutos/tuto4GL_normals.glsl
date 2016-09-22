
#version 330

#ifdef VERTEX_SHADER
in vec3 position;
in vec3 normal;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

// normal est un attribut du vertex shader, il n'est pas disponible dans le fragment shader, donc :
out vec3 view_normal;
// il faut declarer un varying, un resultat du vertex shader

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    view_normal= mat3(mvMatrix) * normal;   // uniquement une rotation, mat3 suffit 
    
}
#endif

#ifdef FRAGMENT_SHADER
uniform vec3 color;

// recupere la normale calculee par le vertex shader, meme type, meme nom, mais in au lieu de out
in vec3 view_normal;
// rappel: interpolation en fonction de la position du fragment dans le triangle

out vec4 fragment_color;

void main( )
{
    fragment_color= vec4(color * abs(view_normal.z), 1);
}
#endif
