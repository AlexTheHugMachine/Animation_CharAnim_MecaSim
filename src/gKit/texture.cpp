
#include <cassert>
#include <cstdio>

#include "texture.h"


GLuint make_texture( const int unit, const Image& im )
{
    if(im.data.size() == 0)
        return 0;
    
    // cree la texture openGL
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // choisit la representation des pixels de la texture en fonction de l'image
    GLenum data_format= 0;
    switch(im.channels)
    {
        case 1: data_format= GL_RED; break;
        case 2: data_format= GL_RG; break;
        case 3: data_format= GL_RGB; break;
        case 4: data_format= GL_RGBA; break;
        default: assert(0 && "invalid image format");
    }
    
    // fixe les parametres de filtrage par defaut
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // transfere les donnees dans la texture
    glTexImage2D(GL_TEXTURE_2D, 0, 
        GL_RGBA, im.width, im.height, 0,
        data_format, GL_UNSIGNED_BYTE, &im.data.front());
    
    // prefiltre la texture
    glGenerateMipmap(GL_TEXTURE_2D);
    return texture;
}


GLuint read_texture( const int unit, const char *filename )
{
    Image im= read_image(filename);
    GLuint texture= make_texture(unit, im);
    release_image(im);
    return texture;
}


int screenshot( const char *filename )
{
    // recupere le contenu de la fenetre / framebuffer par defaut
    glFinish();
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glReadBuffer(GL_BACK);
    
    // recupere les dimensions de la fenetre 
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    // cree une image pour stocker le resultat
    Image image= create_image(viewport[2], viewport[3], 4, make_color(0, 0, 0));
    // transfere les pixels
    glReadPixels(0, 0, viewport[2], viewport[3], 
        GL_RGBA, GL_UNSIGNED_BYTE, (void *) &image.data.front());
    
    // ecrit l'image
    int code= write_image(image, filename);
    release_image(image);
    return code;
}

int capture( const char *prefix )
{
    static int id= 1;

    char tmp[4096];
    sprintf(tmp,"%s%04d.bmp", prefix, id);

    if(id % 30 == 0)
        printf("capture frame '%s'...\n", tmp);

    id++;
    return screenshot(tmp);
}

