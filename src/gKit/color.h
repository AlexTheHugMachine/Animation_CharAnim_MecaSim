
#ifndef _COLOR_H
#define _COLOR_H


//! \addtogroup image
///@{

//! \file 
//! manipulation de couleurs

//! representation d'une couleur (rgba) transparente ou opaque.
struct Color
{
    float r, g, b, a;
};

//! construit une couleur opaque.
Color make_color( const float r, const float g, const float b );
//! construit une couleur transparente.
Color make_alpha_color( const float r, const float g, const float b, const float a );
//! renvoie la couleur opaque (color.rgb, 1).
Color make_opaque_color( const Color& color );

//! utilitaire. renvoie une couleur noire.
Color make_black( );
//! utilitaire. renvoie une couleur blanche.
Color make_white( );
//! utilitaire. renvoie une couleur rouge.
Color make_red( );
//! utilitaire. renvoie une couleur verte.
Color make_green( );
//! utilitaire. renvoie une couleur bleue.
Color make_blue( );


Color operator+ ( const Color& a, const Color& b );
Color operator- ( const Color& a, const Color& b );
Color operator- ( const Color& c );
Color operator* ( const Color& a, const Color& b );
Color operator* ( const Color& c, const float k );
Color operator* ( const float k, const Color& c );
Color operator/ ( const Color& a, const Color& b );
Color operator/ ( const float k, const Color& c );
Color operator/ ( const Color& c, const float k );

///@}
#endif
