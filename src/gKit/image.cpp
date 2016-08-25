
#include "image.h"


Image create_image( const int w, const int h, const Color& color )
{
    return Image(w, h, color);
}

void release_image( Image& im )
{
    //~ im.clear();
}

Color image_pixel( const Image& im, const int x, const int y )
{
    return im(x, y);
}

void image_set_pixel( Image& im, const int x, const int y, const Color& color )
{
    im(x, y)= color;
}

