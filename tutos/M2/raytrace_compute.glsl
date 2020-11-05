
#version 430

#ifdef COMPUTE_SHADER

struct Triangle
{
    vec3 a;		// sommet
    vec3 ab;	// arete 1
    vec3 ac;	// arete 2
};

// shader storage buffer 0
layout(std430, binding= 0) readonly buffer triangleData
{
    Triangle triangles[];
};


bool intersect( const Triangle triangle, const vec3 o, const vec3 d, const float tmax, out float rt, out float ru, out float rv )
{
    vec3 pvec= cross(d, triangle.ac);
    float det= dot(triangle.ab, pvec);
    float inv_det= 1.0f / det;
    
    vec3 tvec= o - triangle.a;
    float u= dot(tvec, pvec) * inv_det;
    vec3 qvec= cross(tvec, triangle.ab);
    float v= dot(d, qvec) * inv_det;
    
    /* calculate t, ray intersects triangle */
    rt= dot(triangle.ac, qvec) * inv_det;
    ru= u;
    rv= v;
    
    // ne renvoie vrai que si l'intersection est valide : 
    // interieur du triangle, 0 < u < 1, 0 < v < 1, 0 < u+v < 1
    if(any(greaterThan(vec3(u, v, u+v), vec3(1, 1, 1))) || any(lessThan(vec2(u, v), vec2(0, 0))))
        return false;
    // comprise entre 0 et tmax du rayon
    return (rt < tmax && rt > 0);
}

uniform mat4 invMatrix;

// image resultat
layout(binding= 0, rgba8)  writeonly uniform image2D image;

// 8x8 threads
layout( local_size_x= 8, local_size_y= 8 ) in;
void main( )
{
    // recupere le threadID 2d, et l'utilise directement comme coordonnees de pixel
    vec2 position= vec2(gl_GlobalInvocationID.xy);
    
    // construction du rayon pour le pixel, passage depuis le repere image vers le repere monde
    vec4 oh= invMatrix * vec4(position, 0, 1);       // origine sur near
    vec4 eh= invMatrix * vec4(position, 1, 1);       // extremite sur far

    // origine et direction
    vec3 o= oh.xyz / oh.w;                              // origine
    vec3 d= eh.xyz / eh.w - oh.xyz / oh.w;              // direction

    float hit= 1;	// tmax = far, une intersection valide est plus proche que l'extremite du rayon / far...
    float hitu= 0;
    float hitv= 0;
    for(int i= 0; i < triangles.length(); i++)
    {
        float t, u, v;
        if(intersect(triangles[i], o, d, hit, t, u, v))
        {
            hit= t;
            hitu= u;
            hitv= v;
        }
    }
    
    // ecrire le resultat dans l'image
    imageStore(image, ivec2(gl_GlobalInvocationID.xy), vec4(hitu, hitv, 0, 1));
}
#endif
