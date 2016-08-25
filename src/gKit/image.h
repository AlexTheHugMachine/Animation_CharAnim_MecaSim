
#ifndef _IMAGE_H
#define _IMAGE_H

#include <vector>
#include <cassert>

#include "color.h"


//! \addtogroup image utilitaires pour manipuler des images
///@{

//! \file
//! manipulation d'images

//! representation d'une image.
class Image
{
protected:
    std::vector<Color> data;

public:
    Image( ) : data(), width(0), height(0) {}
    Image( const int w, const int h, const Color& color= make_black() ) : data(w*h, color), width(w), height(h) {}
    
    /*! renvoie une reference sur la couleur d'un pixel de l'image.
    permet de modifier et/ou de connaitre la couleur d'un pixel :
    \code
    Image image(512, 512);
    
    image(10, 10)= make_red();      // le pixel (10, 10) devient rouge
    image(0, 0)= image(10, 10);     // le pixel (0, 0) recupere la couleur du pixel (10, 10)
    \endcode
    */
    Color& operator() ( const int x, const int y )
    {
        std::size_t offset= y * width + x;
        assert(offset < data.size());
        return data[offset];
    }
    
    //! renvoie la couleur d'un pixel de l'image (image non modifiable).
    Color operator() ( const int x, const int y ) const
    {
        std::size_t offset= y * width + x;
        assert(offset < data.size());
        return data[offset];
    }
    
    const void * buffer( ) const
    {
        assert(!data.empty());
        return &data.front();
    }
    
    int width;
    int height;
    
    
    /*! sentinelle pour la gestion d'erreur lors du chargement d'un fichier.
    exemple :
    \code
    Image image= read_image("debug.png");
    if(image == Image::error())
        return "erreur de chargement";
    \endcode
    */
    static Image& error( )
    {
        static Image image;
        return image;
    }
    
    //! comparaison avec la sentinelle. \code if(image == Image::error()) { ... } \endcode
    bool operator== ( const Image& im ) const
    {
        // renvoie vrai si l'image est la sentinelle
        return (this == &im);
    }
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

//! renvoie la couleur d'un pixel de l'image.
Color image_pixel( const Image& im, const int x, const int y );
//! modife la couleur d'un pixel de l'image.
void image_set_pixel( Image& im, const int x, const int y, const Color& color );

///@}
#endif
