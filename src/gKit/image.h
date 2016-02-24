
#ifndef _IMAGE_H
#define _IMAGE_H

#include <vector>

#include "color.h"


//! representation d'une image.
struct Image
{
    std::vector<unsigned char> data;
    int width;
    int height;
    int channels;
    // levels
};

//! cree une image de dimensions largeur \param width, hauteur \param height. les pixels sont representes par \param channels valeurs. l'image est initialisee avec la couler \param color.
Image create_image( const int width, const int height, const int channels, const Color& color );
//! detruit l'image.
void release_image( Image& im );

//! cree une image initialisee avec le contenu d'un fichier \param filemane. renvoie une image rouge en cas d'echec.
Image read_image( const char *filename );

//! enregistre une image dans un fichier png.
int write_image( const Image& image, const char *filename );

//! renvoie la couleur d'un pixel de l'image.
Color image_pixel( const Image& im, const int x, const int y );
//! modife la couleur d'un pixel de l'image.
void image_set_pixel( Image& im, const int x, const int y, const Color& color );

#endif
