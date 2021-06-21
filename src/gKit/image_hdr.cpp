
#include <chrono>
#include <cstring>
#include <string>

#include "rgbe.h"
#include "image_hdr.h"


bool is_hdr_image( const char *filename )
{
    return (std::string(filename).rfind(".hdr") != std::string::npos);
}


Image read_image_hdr( const char *filename )
{
    FILE *in= fopen(filename, "rb");
    if(in == NULL)
    {
        printf("[error] loading hdr image '%s'...\n", filename);
        return Image::error();
    }

    int width, height;
    rgbe_header_info info;
    if(RGBE_ReadHeader(in, &width, &height, &info) != RGBE_RETURN_SUCCESS)
    {
        fclose(in);
        printf("[error] loading hdr image '%s'...\n", filename);
        return Image::error();
    }

    std::vector<float> data(width*height*3, 0.f);
    if(RGBE_ReadPixels_RLE(in, &data.front(), width, height) != RGBE_RETURN_SUCCESS)
    {
        fclose(in);
        printf("[error] loading hdr image '%s'...\n", filename);
        return Image::error();
    }

    fclose(in);
    
    // 
    printf("loading hdr image '%s' %dx%d...\n", filename, width, height);
    Image image(width, height);
    
    int i= 0;
    for(int y= 0; y < height; y++)
    for(int x= 0; x < width; x++, i+= 3)
        image(x, height - y -1)= Color(data[i], data[i+1], data[i+2]);
    
    return image;
}

int write_image_hdr( const Image& image, const char *filename )
{
    if(image == Image::error())
        return -1;
    
    FILE *out= fopen(filename, "wb");
    if(out == NULL)
    {
        printf("[error] writing hdr image '%s'...\n", filename);
        return -1;
    }

    int width= image.width();
    int height= image.height();
    if(RGBE_WriteHeader(out, width, height, NULL) != RGBE_RETURN_SUCCESS)
    {
        fclose(out);

        printf("[error] writing hdr image '%s'...\n", filename);
        return -1;
    }

    std::vector<float> data(width*height*3, 0.f);
    int i= 0;
    for(int y= 0; y < height; y++)
    for(int x= 0; x < width; x++, i+= 3)
    {
        Color color= image(x, height - y -1);
        data[i]= color.r;
        data[i+1]= color.g;
        data[i+2]= color.b;
    }

    int code= RGBE_WritePixels_RLE(out, &data.front(), width, height);
    fclose(out);

    if(code != RGBE_RETURN_SUCCESS)
    {
        printf("[error] writing hdr image '%s'...\n", filename);
        return -1;
    }

    printf("writing hdr image '%s'...\n", filename);
    return 0;
}


Image read_image_pfm( const char *filename )
{
    FILE *in= fopen(filename, "rb");
    if(in == nullptr)
    {
        printf("[error] loading pfm image '%s'...\n", filename);
        return Image();
    }
    
    int w, h;
    float endian= 0;
    if(fscanf(in, "PF\xa%d %d\xa%f[^\xa]", &w, &h, &endian) != 3 
    || endian != -1)
    {
        printf("[error] loading pfm image '%s'...\n", filename);
        return Image();
    }
    
    // saute la fin de l'entete
    unsigned char c= fgetc(in);
    while(c != '\xa')
        c= fgetc(in);
    
    // pourquoi aussi tordu ? fscanf(in, "PF\n%d %d\n%f\n") consomme les espaces apres le \n... ce qui est un poil genant pour relire les floats...
    
    printf("loading pfm image '%s' %dx%d...\n", filename, w, h);

    auto cpu_start= std::chrono::high_resolution_clock::now();
    
#if 0
    Image image(w, h);
    
    for(int y= 0; y < h; y++)
    for(int x= 0; x < w; x++)
    {
        Color pixel;
        if(fread(&pixel.r, sizeof(float), 3, in) == 3)
            image(x, y)= pixel;
    }
    // ~17ms pour une image 1024x1024
#else
    
    const int n= w*h;
    float *tmp= (float *) malloc(n*sizeof(float)*3);
    if(fread(tmp, sizeof(float)*3, n, in) != size_t(n))
        printf("[error] reading pfm image '%s'...\n", filename);

    Image image(w, h);
    
    for(int i= 0; i < n; i++)
        image(i)= Color(tmp[3*i], tmp[3*i+1], tmp[3*i+2]);     // 2ms
#endif
    
    fclose(in);
    
    auto cpu_stop= std::chrono::high_resolution_clock::now();
    auto cpu_time= std::chrono::duration_cast<std::chrono::milliseconds>(cpu_stop - cpu_start).count();
    int kb= w*h*3*sizeof(float) / 1024;
    int mb= kb / 1024;
    printf("cpu  %ds %03dms %dKB, %.2fMB/s\n", int(cpu_time / 1000), int(cpu_time % 1000), kb, float(mb) / (float(cpu_time) / 1000.f));

    return image;
}


//! enregistre une image dans un fichier .pfm.
int write_image_pfm( const Image& image, const char *filename )
{
    FILE *out= fopen(filename, "wb");
    if(out == nullptr)
    {
        printf("[error] writing pfm image '%s'...\n", filename);
        return -1;
    }

    fprintf(out, "PF\xa%d %d\xa-1\xa", image.width(), image.height());
    
    //~ auto cpu_start= std::chrono::high_resolution_clock::now();
    
#if 0
    for(int y= 0; y < image.height(); y++)
    for(int x= 0; x < image.width(); x++)
    {
        Color pixel= image(x, y);
        fwrite(&pixel.r, sizeof(float), 3, out);
    }
#else

    const int w= image.width();
    const int h= image.height();
    std::vector<float> pixels(w*h*3);
    
    int i= 0;
    for(int y= 0; y < h; y++)
    for(int x= 0; x < w; x++, i+= 3)
    {
        Color pixel= image(x, y);
        pixels[i]= pixel.r;
        pixels[i+1]= pixel.g;
        pixels[i+2]= pixel.b;
    }
    
    fwrite(pixels.data(), sizeof(float)*3, w*h, out);
#endif

    fclose(out);
    
    //~ auto cpu_stop= std::chrono::high_resolution_clock::now();
    //~ auto cpu_time= std::chrono::duration_cast<std::chrono::milliseconds>(cpu_stop - cpu_start).count();
    //~ printf("cpu  %ds %03dms\n", int(cpu_time / 1000), int(cpu_time % 1000));

    printf("writing pfm image '%s'...\n", filename);
    return 0;
}


//! renvoie vrai si le nom de fichier se termine par .pfm.
bool is_pfm_image( const char *filename )
{
    return (std::string(filename).rfind(".pfm") != std::string::npos);
}
