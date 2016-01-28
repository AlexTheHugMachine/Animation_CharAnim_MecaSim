
#include <cstdio>

#include "SDL2/SDL_image.h"
#include "image.h"



image create_image( const int w, const int h, const int c, const vec4& color )
{
    image im;
    im.width= w;
    im.height= h;
    im.channels= c;

#if 0
    unsigned char *data= new unsigned char[w*h*c];
    im.data= data;
#else
    im.data.resize(w*h*c);
    unsigned char *data= &im.data.front();
#endif

    unsigned char r= color.x * 255.f;
    unsigned char g= color.y * 255.f;
    unsigned char b= color.z * 255.f;
    unsigned char a= color.w * 255.f;

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

void release_image( image& im )
{
#if 0
    delete im.data;
    im.data= NULL;
#else
    im.data.clear();
#endif
}


vec4 image_pixel( image& im, const int x, const int y )
{
#if 0
    if(im.data == NULL)
        return make_vec4(0, 0, 0, 1);
#else
    if(im.data.size() == 0)
        return make_vec4(0, 0, 0, 1);
#endif

    int offset= im.width * y * im.channels + x * im.channels;
    unsigned char *data= &im.data.front() + offset;

    if(im.channels > 3)
        return make_vec4(data[0] / 255.f, data[1] / 255.f, data[2] / 255.f, data[3] / 255.f);
    else
        return make_vec4(data[0] / 255.f, data[1] / 255.f, data[2] / 255.f, 1);
}

void image_set_pixel( image& im, const int x, const int y, const vec4& color )
{
#if 0
    if(im.data == NULL)
        return;
#else
    if(im.data.size() == 0)
        return;
#endif

    int offset= im.width * y * im.channels + x * im.channels;
    unsigned char *data= &im.data.front() + offset;

    data[0]= color.x * 255.f;
    data[1]= color.y * 255.f;
    data[2]= color.z * 255.f;
    if(im.channels > 3)
        data[3]= color.w * 255.f;
}


image read_image( const char *filename )
{
    printf("loading image '%s'...\n", filename);

    // importer le fichier en utilisant SDL_image
    SDL_Surface *surface= IMG_Load(filename);
    if(surface == NULL)
    {
        printf("loading image '%s'... sdl_image failed.\n", filename);
        return create_image(2, 2, 4, make_vec4(1, 0, 0, 1));
    }

    // verifier le format, rgb ou rgba
    const SDL_PixelFormat format= *surface->format;
    if(format.BitsPerPixel != 24 && format.BitsPerPixel != 32)
    {
        printf("loading image '%s'... format failed. (bpp %d)\n", filename, format.BitsPerPixel);
        SDL_FreeSurface(surface);
        return create_image(2, 2, 4, make_vec4(1, 0, 0, 1));
    }

    int height= surface->h;
    int width= surface->w;
    int channels= (format.BitsPerPixel == 32) ? 4 : 3;

    image im;
#if 0
    unsigned char *data= new unsigned char[width*height*channels];
    im.data= data;
#else
    im.data.resize(width*height*channels);
#endif
    im.width= width;
    im.height= height;
    im.channels= channels;

    // converti les donnees en pixel rgba, et retourne l'image, openGL utilise une origine en bas a gauche.
    if(format.BitsPerPixel == 32)
    {
        int py= 0;
        for(int y= height -1; y >= 0; y--, py++)
        {
        #if 0
            unsigned char *data= im.data + im.width * y * im.channels;
        #else
            unsigned char *data= &im.data.front() + im.width * y * im.channels;
        #endif
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
        #if 0
            unsigned char *data= im.data + im.width * y * im.channels;
        #else
            unsigned char *data= &im.data.front() + im.width * y * im.channels;
        #endif
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

