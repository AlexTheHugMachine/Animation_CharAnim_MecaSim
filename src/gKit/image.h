
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
//~ protected:
    std::vector<unsigned char> data;

public:
    Image( ) : data(), width(0), height(0), channels(0) {}
    Image( const int width, const int height, const int channels, const Color& color= make_black() );
    ~Image( ) {}

    //! renvoie la couleur d'un pixel de l'image.
    Color pixel( const int x, const int y ) const;

    //! modifie la couleur d'un pixel de l'image.
    void pixel( const int x, const int y, const Color& color );

    //! renvoie le nombre de niveaux de mipmaps de l'image, cf textures openGL.
    int miplevels( ) const;
    
    int width;          //!< largeur.
    int height;         //!< hauteur.
    int channels;       //!< nombre de canaux couleur.
    
    /*! sentinelle pour la gestion d'erreur.
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
    
    //! comparaison avec la sentinelle.
    bool operator== ( const Image& im ) const
    {
        return &im == &error();
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
