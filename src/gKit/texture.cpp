
#include <assert.h>

#include "texture.h"


GLuint make_texture( const int unit, const Image& im )
{
    if(im.data.size() == 0)
        return 0;
    
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    GLenum data_format= 0;
    switch(im.channels)
    {
        case 1: data_format= GL_RED; break;
        case 2: data_format= GL_RG; break;
        case 3: data_format= GL_RGB; break;
        case 4: data_format= GL_RGBA; break;
        default: assert(0 && "invalid image format");
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, 
        GL_RGBA, im.width, im.height, 0,
        data_format, GL_UNSIGNED_BYTE, &im.data.front());
    
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

