
#include <cassert>
#undef __STRICT_ANSI__
#include <cmath>
#include <cstdio>

#include "mat.h"


float radians( const float deg )
{
    return ((float) M_PI / 180.f) * deg;
}

float degrees( const float rad )
{
    return (180.f / (float) M_PI) * rad;
}


mat4 make_matrix( const vec4& r0, const vec4& r1, const vec4& r2, const vec4& r3 )
{
    mat4 m;
    m.m[0][0]= r0.x; m.m[0][1]= r0.y; m.m[0][2]= r0.z; m.m[0][3]= r0.w;
    m.m[1][0]= r1.x; m.m[1][1]= r1.y; m.m[1][2]= r1.z; m.m[1][3]= r1.w;
    m.m[2][0]= r2.x; m.m[2][1]= r2.y; m.m[2][2]= r2.z; m.m[2][3]= r2.w;
    m.m[3][0]= r3.x; m.m[3][1]= r3.y; m.m[3][2]= r3.z; m.m[3][3]= r3.w;
    return m;
}

mat4 make_matrix(
    const float t00, const float t01, const float t02, const float t03,
    const float t10, const float t11, const float t12, const float t13,
    const float t20, const float t21, const float t22, const float t23,
    const float t30, const float t31, const float t32, const float t33 )
{
    mat4 m;
    m.m[0][0]= t00; m.m[0][1]= t01; m.m[0][2]= t02; m.m[0][3]= t03;
    m.m[1][0]= t10; m.m[1][1]= t11; m.m[1][2]= t12; m.m[1][3]= t13;
    m.m[2][0]= t20; m.m[2][1]= t21; m.m[2][2]= t22; m.m[2][3]= t23;
    m.m[3][0]= t30; m.m[3][1]= t31; m.m[3][2]= t32; m.m[3][3]= t33;
    return m;
}

mat4 make_identity( )
{
    return make_matrix(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
}

mat4 make_transpose( const mat4& m )
{
    return make_matrix(
        m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
        m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
        m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
        m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]);
}

mat4 make_normal_matrix( const mat4& m )
{
    return make_transpose( make_inverse(m) );
}


mat4 make_scale( const float x, const float y, const float z )
{
    return make_matrix(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1);
}

mat4 make_translation( const vec3& v )
{
    return make_matrix(
        1, 0, 0, v.x,
        0, 1, 0, v.y,
        0, 0, 1, v.z,
        0, 0, 0, 1);
}

mat4 make_rotationX( const float a )
{
    float sin_t= sinf(radians(a));
    float cos_t= cosf(radians(a));

    return make_matrix(
        1,     0,      0, 0,
        0, cos_t, -sin_t, 0,
        0, sin_t,  cos_t, 0,
        0,     0,      0, 1 );
}

mat4 make_rotationY( const float a )
{
    float sin_t= sinf(radians(a));
    float cos_t= cosf(radians(a));

    return make_matrix(
         cos_t,   0, sin_t, 0,
             0,   1,     0, 0,
        -sin_t,   0, cos_t, 0,
             0,   0,     0, 1 );
}

mat4 make_rotationZ( const float a )
{
    float sin_t= sinf(radians(a));
    float cos_t= cosf(radians(a));

    return make_matrix(
        cos_t, -sin_t, 0, 0,
        sin_t,  cos_t, 0, 0,
            0,      0, 1, 0,
            0,      0, 0, 1 );
}

mat4 make_rotation( const vec3& axis, const float angle )
{
    vec3 a= normalize(axis);
    float s= sinf(radians(angle));
    float c= cosf(radians(angle));

    vec4 m[4];

    m[0]= make_vec4(
        a.x * a.x + (1 - a.x * a.x ) * c,
        a.x * a.y * (1 - c ) - a.z * s,
        a.x * a.z * (1 - c ) + a.y * s,
        0);

    m[1]= make_vec4(
        a.x * a.y * (1 - c ) + a.z * s,
        a.y * a.y + (1 - a.y * a.y ) * c,
        a.y * a.z * (1 - c ) - a.x * s,
        0);

    m[2]= make_vec4(
        a.x * a.z * (1 - c ) - a.y * s,
        a.y * a.z * (1 - c ) + a.x * s,
        a.z * a.z + (1 - a.z * a.z ) * c,
        0);

    m[3]= make_vec4(0, 0, 0, 1);

    return make_matrix(m[0], m[1], m[2], m[3]);
}


mat4 make_perspective( const float fov, const float aspect, const float znear, const float zfar )
{
    // perspective, openGL version
    float inv_tan= 1.f / tanf(radians(fov) * 0.5f);
    float inv_denom= 1.f / (znear - zfar);

    return make_matrix(
        inv_tan/aspect,       0,                    0,                      0,
                     0, inv_tan,                    0,                      0,
                     0,       0, (zfar+znear)*inv_denom, 2.f*zfar*znear*inv_denom,
                     0,       0,                   -1,                      0);
}

mat4 make_viewport( const float width, const float height )
{
    float w= width / 2.f;
    float h= height / 2.f;

    return make_matrix(
        w, 0,   0,   w,
        0, h,   0,   h,
        0, 0, .5f, .5f,
        0, 0,   0,   1);
}


mat4 make_lookat( const vec3& from, const vec3& to, const vec3& up )
{
    vec3 dir = normalize( to - from );
    vec3 right = normalize( cross(dir, normalize(up)) );
    vec3 newUp = normalize( cross(right, dir) );

    mat4 m= make_matrix(
        right.x, newUp.x, -dir.x, from.x,
        right.y, newUp.y, -dir.y, from.y,
        right.z, newUp.z, -dir.z, from.z,
        0,       0,        0,     1);

    return make_inverse(m);
}


mat4 make_transform( const mat4& a, const mat4& b )
{
    mat4 m;
    for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
        m.m[i][j]= a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];

    return m;
}

mat4 operator* ( const mat4& a, const mat4& b )
{
    return make_transform(a, b);
}


static
void swapf( float& a, float& b )
{
    float c= a;
    a= b;
    b= c;
}

mat4 make_inverse( const mat4& m )
{
    int indxc[4], indxr[4];
    int ipiv[4] = { 0, 0, 0, 0 };

    mat4 minv= m;

    for (int i = 0; i < 4; i++) {
        int irow = -1, icol = -1;
        float big = 0.f;

        // Choose pivot
        for (int j = 0; j < 4; j++) {
            if (ipiv[j] != 1) {
                for (int k = 0; k < 4; k++) {
                    if (ipiv[k] == 0) {
                        if (fabsf(minv.m[j][k]) >= big) {
                            big = fabsf(minv.m[j][k]);
                            irow = j;
                            icol = k;
                        }
                    }
                    else if (ipiv[k] > 1)
                        printf("singular matrix in make_inverse()\n");
                }
            }
        }

        assert(irow >= 0 && irow < 4);
        assert(icol >= 0 && icol < 4);

        ++ipiv[icol];
        // Swap rows _irow_ and _icol_ for pivot
        if (irow != icol) {
            for (int k = 0; k < 4; ++k)
                swapf(minv.m[irow][k], minv.m[icol][k]);
        }

        indxr[i] = irow;
        indxc[i] = icol;
        if (minv.m[icol][icol] == 0.)
            printf("singular matrix in make_inverse()\n");

        // Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
        float pivinv = 1.f / minv.m[icol][icol];
        minv.m[icol][icol] = 1.f;
        for (int j = 0; j < 4; j++)
            minv.m[icol][j] *= pivinv;

        // Subtract this row from others to zero out their columns
        for (int j = 0; j < 4; j++) {
            if (j != icol) {
                float save = minv.m[j][icol];
                minv.m[j][icol] = 0;
                for (int k = 0; k < 4; k++)
                    minv.m[j][k] -= minv.m[icol][k]*save;
            }
        }
    }

    // Swap columns to reflect permutation
    for (int j = 3; j >= 0; j--) {
        if (indxr[j] != indxc[j]) {
            for (int k = 0; k < 4; k++)
                swapf(minv.m[k][indxr[j]], minv.m[k][indxc[j]]);
        }
    }

    return minv;
}


//~ vec4 operator() ( const mat4& m, const vec4& v )
//~ {
    //~ float x= v.x;
    //~ float y= v.y;
    //~ float z= v.z;
    //~ float w= v.w;

    //~ float xt= m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3] * w;
    //~ float yt= m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3] * w;
    //~ float zt= m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3] * w;
    //~ float wt= m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3] * w;

    //~ return make_vec4(xt, yt, zt, wt);
//~ }

vec4 transform( const mat4& m, const vec4& v )
{
    float x= v.x;
    float y= v.y;
    float z= v.z;
    float w= v.w;

    float xt= m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3] * w;    // dot(vec4(m[0]), v)
    float yt= m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3] * w;    // dot(vec4(m[1]), v)
    float zt= m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3] * w;    // dot(vec4(m[2]), v)
    float wt= m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3] * w;    // dot(vec4(m[3]), v)

    return make_vec4(xt, yt, zt, wt);
}

vec3 transform_point( const mat4& m, const vec3& p )
{
    float x= p.x;
    float y= p.y;
    float z= p.z;

    float xt= m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];        // dot(vec4(m[0]), vec4(p, 1))
    float yt= m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];        // dot(vec4(m[1]), vec4(p, 1))
    float zt= m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];        // dot(vec4(m[2]), vec4(p, 1))
    float wt= m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];        // dot(vec4(m[3]), vec4(p, 1))

    assert(wt != 0);
    float w= 1 / wt;
    if(wt == 1)
        return make_vec3(xt, yt, zt);
    else
        return make_vec3(xt*w, yt*w, zt*w);
}

vec3 transform_vector( const mat4& m, const vec3& v )
{
    float x= v.x;
    float y= v.y;
    float z= v.z;

    float xt= m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z;        // dot(vec4(m[0]), vec4(v, 0))
    float yt= m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z;        // dot(vec4(m[1]), vec4(v, 0))
    float zt= m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z;        // dot(vec4(m[2]), vec4(v, 0))
                                                                    // dot(vec4(m[3]), vec4(v, 0)) == dot(vec4(0, 0, 0, 1), vec4(v, 0)) == 0 par definition
    return make_vec3(xt, yt, zt);
}

vec3 transform_normal( const mat4& m, const vec3& n )
{
    //! \todo transformation par la transpose de l'inverse de m...
    float x= n.x;
    float y= n.y;
    float z= n.z;

    float xt= m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z;
    float yt= m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z;
    float zt= m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z;

    return make_vec3(xt, yt, zt);
}


