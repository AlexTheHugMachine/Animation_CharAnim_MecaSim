
#include "color.h"


Color make_color( const float r, const float g, const float b )
{
    return Color(r, b, g, 1.f);
}

Color make_alpha_color( const float r, const float g, const float b, const float a )
{
    return Color(r, g, b, a);
}

Color make_opaque_color( const Color& color )
{
    return Color(color, 1.f);
}

Color Black( )
{
    return Color(0, 0, 0);
}

Color White( )
{
    return Color(1, 1, 1);
}

Color Red( )
{
    return Color(1, 0, 0);
}

Color Green( )
{
    return Color(0, 1, 0);
}

Color Blue( )
{
    return Color(0, 0, 1);
}


Color operator+ ( const Color& a, const Color& b )
{
    return Color(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
}

Color operator- ( const Color& c )
{
    return Color(-c.r, -c.g, -c.b, -c.a);
}

Color operator- ( const Color& a, const Color& b )
{
    return a + (-b);
}

Color operator* ( const Color& a, const Color& b )
{
    return Color(a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a);
}

Color operator* ( const float k, const Color& c )
{
    return Color(c.r * k, c.g * k, c.b * k, c.a * k);
}

Color operator* ( const Color& c, const float k )
{
    return k * c;
}

Color operator/ ( const Color& a, const Color& b )
{
    return Color(a.r / b.r, a.g / b.g, a.b / b.b, a.a / b.a);
}

Color operator/ ( const float k, const Color& c )
{
    return Color(k / c.r, k / c.g, k / c.b, k / c.a);
}

Color operator/ ( const Color& c, const float k )
{
    float kk= 1 / k;
    return kk * c;
}
