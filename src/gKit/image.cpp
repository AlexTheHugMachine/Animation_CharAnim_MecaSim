
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
    init(w,h,c,color);
}

void Image::init( const int w, const int h, const int c, const Color& color )
{
    clear();

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


Image::Image( const char *filename )
{
    width = height = channels = 0;

        // importer le fichier en utilisant SDL_image
    SDL_Surface *surface= IMG_Load(filename);
    if(surface == NULL)
    {
        printf("loading image '%s'... sdl_image failed.\n", filename);
        //return create_image(2, 2, 4, make_color(1, 0, 0));
        return;
    }

    // verifier le format, rgb ou rgba
    const SDL_PixelFormat format= *surface->format;
    if(format.BitsPerPixel != 24 && format.BitsPerPixel != 32)
    {
        printf("loading image '%s'... format failed. (bpp %d)\n", filename, format.BitsPerPixel);
        SDL_FreeSurface(surface);
        //return create_image(2, 2, 4, make_color(1, 0, 0));
        return;
    }

    height= surface->h;
    width= surface->w;
    channels= (format.BitsPerPixel == 32) ? 4 : 3;

    printf("loading image '%s' %dx%d %d channels...\n", filename, width, height, channels);

    init(width,height,channels);

    // converti les donnees en pixel rgba, et retourne l'image, openGL utilise une origine en bas a gauche.
    if(format.BitsPerPixel == 32)
    {
        int py= 0;
        for(int y= height -1; y >= 0; y--, py++)
        {
            unsigned char *dat= &data.front() + width * y * channels;
            Uint8 *pixel= (Uint8 *) surface->pixels + py * surface->pitch;

            for(int x= 0; x < width; x++, pixel+= format.BytesPerPixel, dat+= 4)
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
        for(int y= height -1; y >= 0; y--, py++)
        {
            unsigned char *dat= &data.front() + width * y * channels;
            Uint8 *pixel= (Uint8 *) surface->pixels + py * surface->pitch;

            for(int x= 0; x < width; x++, pixel+= format.BytesPerPixel, dat+= 3)
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
}


Color Image::operator()(const int x, const int y) const
{
    return get_pixel( x,y);
}


int Image::write_image( const char *filename ) const
{
    if(std::string(filename).rfind(".png") == std::string::npos && std::string(filename).rfind(".bmp") == std::string::npos )
    {
        printf("writing color image '%s'... failed, not a .png / .bmp image.\n", filename);
        return -1;
    }

    // flip de l'image : Y inverse entre GL et BMP
    Image flip(width, height, 4, Color(0, 0, 0));
    for(int y= 0; y < height; y++)
        for(int x= 0; x < width; x++)
            flip.set_pixel(x, height - y -1, operator()(x, y));

    SDL_Surface *bmp= SDL_CreateRGBSurfaceFrom((void *) &flip.data.front(),
        width, height,
        32, width * 4,
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


Image create_image( const int w, const int h, const int c, const Color& color )
{
    return Image(w,h,c,color);
}

void release_image( Image& im )
{
    im.clear();
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


Color Image::get_pixel(const int x, const int y ) const
{
    if(data.size() == 0)
        return make_color(0, 0, 0);

    int offset= width * y * channels + x * channels;
    const unsigned char *dat= &data.front() + offset;

    if(channels > 3)
        return make_alpha_color(dat[0] / 255.f, dat[1] / 255.f, dat[2] / 255.f, dat[3] / 255.f);
    else
        return make_color(dat[0] / 255.f, dat[1] / 255.f, dat[2] / 255.f);
}

void Image::set_pixel( const int x, const int y, const Color& color )
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
    return Image(filename);
}


int write_image( const Image& image, const char *filename )
{
    image.write_image(filename);
}
