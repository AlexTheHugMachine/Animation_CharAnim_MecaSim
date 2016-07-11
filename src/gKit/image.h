
#ifndef _IMAGE_H
#define _IMAGE_H

#include <vector>

#include "color.h"


//! \addtogroup image utilitaires pour manipuler des images
///@{

//! \file
//! manipulation d'images

//! representation d'une image.
class Image
{
public:
    Image( const int width, const int height, const int channels, const Color& color=Color() );
    Image( const char *filename );
    ~Image()    { clear(); }
    void clear()    { data.clear(); }

    void init( const int width, const int height, const int channels, const Color& color=Color() );

    int write_image( const char *filename ) const;

    Color operator()(const int x, const int y) const;
    Color get_pixel(const int x, const int y ) const;

    void set_pixel( const int x, const int y, const Color& color );

    int miplevels() const;

    bool isInit() const { return data.size()!=0 && (data.size()==width*height*channels); }
    unsigned char* getData() { return data.data(); }
    const unsigned char* getData() const { return data.data(); }
    int getChannels() const { return channels; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
protected:
    std::vector<unsigned char> data;
    int width;
    int height;
    int channels;
    // levels
};

//! cree une image de couleur uniforme. a detruire avec release_image( ).\n
//! les pixels sont representes par 3 ou 4 valeurs / canaux.
//! \param width largeur
//! \param height  hauteur
//! \param channels 3 ou 4 nombre de canaux couleur, 3 pour des pixels opaques, 4 pour des pixels semi transparents
//! \param color couleur initiale des pixels.
Image create_image( const int width, const int height, const int channels, const Color& color );
//! detruit l'image.
void release_image( Image& im );

//! renvoie le nombre de mipmap d'une image.
int miplevels( const Image& im );
//! renvoie le nombre de mipmap d'une image width x height.
int miplevels( const int width, const int height );

//! charge une image a partir d'un fichier. renvoie une image rouge en cas d'echec. a detruire avec release_image( ).
//! \param filemane nom de l'image a charger
Image read_image( const char *filename );

//! enregistre une image dans un fichier png.
int write_image( const Image& image, const char *filename );

//! renvoie la couleur d'un pixel de l'image.
Color image_pixel( const Image& im, const int x, const int y );
//! modife la couleur d'un pixel de l'image.
void image_set_pixel( Image& im, const int x, const int y, const Color& color );

///@}
#endif
