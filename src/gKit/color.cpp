
#include "color.h"


Color make_color( const float r, const float g, const float b )
{
    Color color;
    color.r= r; color.g= g; color.b= b; color.a= 1;
    return color;
}

Color make_alpha_color( const float r, const float g, const float b, const float a )
{
    Color color;
    color.r= r; color.g= g; color.b= b; color.a= a;
    return color;
}

Color make_opaque_color( const Color& color )
{
    Color opaque;
    opaque.r= color.r; opaque.g= color.g; opaque.b= color.b; opaque.a= 1;
    return opaque;
}

Color make_black( )
{
    return make_color(0, 0, 0);
}

Color make_white( )
{
    return make_color(1, 1, 1);
}

Color make_red( )
{
    return make_color(1, 0, 0);
}

Color make_green( )
{
    return make_color(0, 1, 0);
}

Color make_blue( )
{
    return make_color(0, 0, 1);
}


Color operator+ ( const Color& a, const Color& b )
{
    return make_alpha_color(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
}

Color operator- ( const Color& c )
{
    return make_alpha_color(-c.r, -c.g, -c.b, -c.a);
}

Color operator- ( const Color& a, const Color& b )
{
    return a + (-b);
}

Color operator* ( const Color& a, const Color& b )
{
    return make_alpha_color(a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a);
}

Color operator* ( const float k, const Color& c )
{
    return make_alpha_color(c.r * k, c.g * k, c.b * k, c.a * k);
}

Color operator* ( const Color& c, const float k )
{
    return k * c;
}

Color operator/ ( const Color& a, const Color& b )
{
    return make_alpha_color(a.r / b.r, a.g / b.g, a.b / b.b, a.a / b.a);
}

Color operator/ ( const float k, const Color& c )
{
    return make_alpha_color(k / c.r, k / c.g, k / c.b, k / c.a);
}

Color operator/ ( const Color& c, const float k )
{
    float kk= 1 / k;
    return kk * c;
}
