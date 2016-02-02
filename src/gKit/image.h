
#ifndef _IMAGE_H
#define _IMAGE_H

#include <vector>

#include "vec.h"


struct Image
{
    std::vector<unsigned char> data;
    int width;
    int height;
    int channels;
};

Image create_image( const int w, const int h, const int c, const vec4& color );
void release_image( Image& im );        // plus necessaire en utilisant un vector au lieu d'un new pour reserver data

Image read_image( const char *filename );

vec4 image_pixel( Image& im, const int x, const int y );
void image_set_pixel( Image& im, const int x, const int y, const vec4& color );

#endif
