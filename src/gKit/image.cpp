
#include <cstdio>
#include <string>
#include <algorithm>

#ifdef GK_MACOS
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

#include "image.h"


Image create_image( const int w, const int h, const int c, const Color& color )
{
    Image im;
    im.width= w;
    im.height= h;
    im.channels= c;

    im.data.resize(w*h*c);
    unsigned char *data= &im.data.front();

    unsigned char r= (unsigned char) std::min(color.r * 255.f, 255.f);
    unsigned char g= (unsigned char) std::min(color.g * 255.f, 255.f);
    unsigned char b= (unsigned char) std::min(color.b * 255.f, 255.f);
    unsigned char a= (unsigned char) std::min(color.a * 255.f, 255.f);

    if(c > 3)
    {
        int i= 0;
        for(int y= 0; y < h; y++)
        for(int x= 0; x < w; x++, i+= 4)
        {
            data[i   ]= r;
            data[i +1]= g;
            data[i +2]= b;
            data[i +3]= a;
        }
    }
    else
    {
        int i= 0;
        for(int y= 0; y < h; y++)
        for(int x= 0; x < w; x++, i+= 3)
        {
            data[i   ]= r;
            data[i +1]= g;
            data[i +2]= b;
        }
    }

    return im;
}

void release_image( Image& im )
{
    im.data.clear();
}


Color image_pixel( const Image& im, const int x, const int y )
{
    if(im.data.size() == 0)
        return make_color(0, 0, 0);

    int offset= im.width * y * im.channels + x * im.channels;
    const unsigned char *data= &im.data.front() + offset;

    if(im.channels > 3)
        return make_alpha_color(data[0] / 255.f, data[1] / 255.f, data[2] / 255.f, data[3] / 255.f);
    else
        return make_color(data[0] / 255.f, data[1] / 255.f, data[2] / 255.f);
}

void image_set_pixel( Image& im, const int x, const int y, const Color& color )
{
    if(im.data.size() == 0)
        return;

    int offset= im.width * y * im.channels + x * im.channels;
    unsigned char *data= &im.data.front() + offset;

    data[0]= (unsigned char) std::min(color.r * 255.f, 255.f);
    data[1]= (unsigned char) std::min(color.g * 255.f, 255.f);
    data[2]= (unsigned char) std::min(color.b * 255.f, 255.f);
    if(im.channels > 3)
        data[3] = (unsigned char) std::min(color.a * 255.f, 255.f);
}


Image read_image( const char *filename )
{
    // importer le fichier en utilisant SDL_image
    SDL_Surface *surface= IMG_Load(filename);
    if(surface == NULL)
    {
        printf("loading image '%s'... sdl_image failed.\n", filename);
        return create_image(2, 2, 4, make_color(1, 0, 0));
    }

    // verifier le format, rgb ou rgba
    const SDL_PixelFormat format= *surface->format;
    if(format.BitsPerPixel != 24 && format.BitsPerPixel != 32)
    {
        printf("loading image '%s'... format failed. (bpp %d)\n", filename, format.BitsPerPixel);
        SDL_FreeSurface(surface);
        return create_image(2, 2, 4, make_color(1, 0, 0));
    }

    int height= surface->h;
    int width= surface->w;
    int channels= (format.BitsPerPixel == 32) ? 4 : 3;

    printf("loading image '%s' %dx%d %d channels...\n", filename, width, height, channels);
    
    Image im;
    im.data.resize(width*height*channels);
    im.width= width;
    im.height= height;
    im.channels= channels;

    // converti les donnees en pixel rgba, et retourne l'image, openGL utilise une origine en bas a gauche.
    if(format.BitsPerPixel == 32)
    {
        int py= 0;
        for(int y= height -1; y >= 0; y--, py++)
        {
            unsigned char *data= &im.data.front() + im.width * y * im.channels;
            Uint8 *pixel= (Uint8 *) surface->pixels + py * surface->pitch;

            for(int x= 0; x < width; x++, pixel+= format.BytesPerPixel, data+= 4)
            {
                Uint8 r= pixel[format.Rshift / 8];
                Uint8 g= pixel[format.Gshift / 8];
                Uint8 b= pixel[format.Bshift / 8];
                Uint8 a= pixel[format.Ashift / 8];

                data[0]= r;
                data[1]= g;
                data[2]= b;
                data[3]= a;
            }
        }
    }

    else if(format.BitsPerPixel == 24)
    {
        int py= 0;
        for(int y= height -1; y >= 0; y--, py++)
        {
            unsigned char *data= &im.data.front() + im.width * y * im.channels;
            Uint8 *pixel= (Uint8 *) surface->pixels + py * surface->pitch;

            for(int x= 0; x < width; x++, pixel+= format.BytesPerPixel, data+= 3)
            {
                const Uint8 r= pixel[format.Rshift / 8];
                const Uint8 g= pixel[format.Gshift / 8];
                const Uint8 b= pixel[format.Bshift / 8];

                data[0]= r;
                data[1]= g;
                data[2]= b;
            }
        }
    }

    SDL_FreeSurface(surface);

    return im;
}


int write_image( const Image& image, const char *filename )
{
    if(std::string(filename).rfind(".png") == std::string::npos && std::string(filename).rfind(".bmp") == std::string::npos )
    {
        printf("writing color image '%s'... failed, not a .png / .bmp image.\n", filename);
        return -1;
    }
    
    // flip de l'image : Y inverse entre GL et BMP
    Image flip= create_image(image.width, image.height, 4, make_color(0, 0, 0));
    for(int y= 0; y < image.height; y++)
    for(int x= 0; x < image.width; x++)
        image_set_pixel(flip, x, image.height - y -1, image_pixel(image, x, y));

    SDL_Surface *bmp= SDL_CreateRGBSurfaceFrom((void *) &flip.data.front(),
        image.width, image.height,
        32, image.width * 4,
#if 0
        0xFF000000,
        0x00FF0000,
        0x0000FF00,
        0x000000FF
#else
        0x000000FF,
        0x0000FF00,
        0x00FF0000,
        0xFF000000
#endif
    );

    int code= 0;
    if(std::string(filename).rfind(".png") != std::string::npos)
        code= IMG_SavePNG(bmp, filename);
    else if(std::string(filename).rfind(".bmp") != std::string::npos)
        code= SDL_SaveBMP(bmp, filename);
    
    SDL_FreeSurface(bmp);
    release_image(flip);
    
    if(code < 0)
        printf("writing color image '%s'... failed\n%s\n", filename, SDL_GetError());
    return code;
}
