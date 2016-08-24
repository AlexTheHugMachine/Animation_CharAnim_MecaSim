
#include <cstdio>
#include <string>
#include <algorithm>

#ifdef GK_MACOS
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

#include "image.h"


Image::Image( const int w, const int h, const int c, const Color& color )
{
    data.clear();

    width= w;
    height= h;
    channels= c;

    data.resize(w*h*c);
    unsigned char *dat = &data.front();

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
            dat[i   ]= r;
            dat[i +1]= g;
            dat[i +2]= b;
            dat[i +3]= a;
        }
    }
    else
    {
        int i= 0;
        for(int y= 0; y < h; y++)
        for(int x= 0; x < w; x++, i+= 3)
        {
            dat[i   ]= r;
            dat[i +1]= g;
            dat[i +2]= b;
        }
    }
}

Image create_image( const int w, const int h, const int c, const Color& color )
{
    return Image(w, h, c, color);
}

void release_image( Image& im )
{
    //~ im.clear();
}


int Image::miplevels() const
{
    int w= width;
    int h= height;
    int levels= 1;
    while(w > 1 || h > 1)
    {
        w= std::max(1, w / 2);
        h= std::max(1, h / 2);
        levels= levels + 1;
    }
    return levels;
}

int miplevels( const Image& im )
{
    return im.miplevels();
}


Color Image::pixel(const int x, const int y ) const
{
    if(data.size() == 0)
        return make_black();

    int offset= width * y * channels + x * channels;
    const unsigned char *dat= &data.front() + offset;

    if(channels > 3)
        return make_alpha_color(dat[0] / 255.f, dat[1] / 255.f, dat[2] / 255.f, dat[3] / 255.f);
    else
        return make_color(dat[0] / 255.f, dat[1] / 255.f, dat[2] / 255.f);
}

void Image::pixel( const int x, const int y, const Color& color )
{
    if(data.size() == 0)
        return;

    int offset= width * y * channels + x * channels;
    unsigned char *dat= &data.front() + offset;

    dat[0]= (unsigned char) std::min(color.r * 255.f, 255.f);
    dat[1]= (unsigned char) std::min(color.g * 255.f, 255.f);
    dat[2]= (unsigned char) std::min(color.b * 255.f, 255.f);
    if(channels > 3)
        dat[3] = (unsigned char) std::min(color.a * 255.f, 255.f);
}


Image read_image( const char *filename )
{
    // importer le fichier en utilisant SDL_image
    SDL_Surface *surface= IMG_Load(filename);
    if(surface == NULL)
    {
        printf("loading image '%s'... sdl_image failed.\n", filename);
        //return create_image(2, 2, 4, make_color(1, 0, 0));
        return Image::error();
    }

    // verifier le format, rgb ou rgba
    const SDL_PixelFormat format= *surface->format;
    if(format.BitsPerPixel != 24 && format.BitsPerPixel != 32)
    {
        printf("loading image '%s'... format failed. (bpp %d)\n", filename, format.BitsPerPixel);
        SDL_FreeSurface(surface);
        //return create_image(2, 2, 4, make_color(1, 0, 0));
        return Image::error();
    }
    
    Image image;
    image.height= surface->h;
    image.width= surface->w;
    image.channels= (format.BitsPerPixel == 32) ? 4 : 3;
    image.data.resize(image.width * image.height * image.channels, 0);

    printf("loading image '%s' %dx%d %d channels...\n", filename, image.width, image.height, image.channels);

    // converti les donnees en pixel rgba, et retourne l'image, openGL utilise une origine en bas a gauche.
    if(format.BitsPerPixel == 32)
    {
        int py= 0;
        for(int y= surface->h -1; y >= 0; y--, py++)
        {
            unsigned char *dat= &image.data.front() + image.width * y * image.channels;
            Uint8 *pixel= (Uint8 *) surface->pixels + py * surface->pitch;

            for(int x= 0; x < surface->w; x++, pixel+= format.BytesPerPixel, dat+= 4)
            {
                Uint8 r= pixel[format.Rshift / 8];
                Uint8 g= pixel[format.Gshift / 8];
                Uint8 b= pixel[format.Bshift / 8];
                Uint8 a= pixel[format.Ashift / 8];

                dat[0]= r;
                dat[1]= g;
                dat[2]= b;
                dat[3]= a;
            }
        }
    }

    else if(format.BitsPerPixel == 24)
    {
        int py= 0;
        for(int y= surface->h -1; y >= 0; y--, py++)
        {
            unsigned char *dat= &image.data.front() + image.width * y * image.channels;
            Uint8 *pixel= (Uint8 *) surface->pixels + py * surface->pitch;

            for(int x= 0; x < surface->w; x++, pixel+= format.BytesPerPixel, dat+= 3)
            {
                const Uint8 r= pixel[format.Rshift / 8];
                const Uint8 g= pixel[format.Gshift / 8];
                const Uint8 b= pixel[format.Bshift / 8];

                dat[0]= r;
                dat[1]= g;
                dat[2]= b;
            }
        }
    }

    SDL_FreeSurface(surface);
    return image;
}


int write_image( const Image& image, const char *filename )
{
    if(std::string(filename).rfind(".png") == std::string::npos && std::string(filename).rfind(".bmp") == std::string::npos )
    {
        printf("writing color image '%s'... failed, not a .png / .bmp image.\n", filename);
        return -1;
    }

    // flip de l'image : Y inverse entre GL et BMP
    Image flip(image.width, image.height, 4, Color(0, 0, 0));
    for(int y= 0; y < flip.height; y++)
        for(int x= 0; x < flip.width; x++)
            flip.pixel(x, flip.height - y -1, image.pixel(x, y));

    SDL_Surface *bmp= SDL_CreateRGBSurfaceFrom((void *) &flip.data.front(),
        flip.width, flip.height,
        32, flip.width * 4,
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

    int code= -1;
    if(std::string(filename).rfind(".png") != std::string::npos)
        code= IMG_SavePNG(bmp, filename);
    else if(std::string(filename).rfind(".bmp") != std::string::npos)
        code= SDL_SaveBMP(bmp, filename);

    SDL_FreeSurface(bmp);
    
    if(code < 0)
        printf("writing color image '%s'... failed\n%s\n", filename, SDL_GetError());
    return code;
}
