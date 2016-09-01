
#ifndef _COLOR_H
#define _COLOR_H


//! \addtogroup image
///@{

//! \file
//! manipulation de couleurs

//! representation d'une couleur (rgba) transparente ou opaque.
struct Color
{
    //! constructeur par defaut.
    Color( const float _r= 0.f, const float _g= 0.f, const float _b= 0.f, const float _a= 1.f ) : r(_r), g(_g), b(_b), a(_a) {}
    //! cree une couleur avec les memes composantes que c, mais remplace sa composante alpha (color.r, color.g, color.b, alpha).
    Color( const Color& color, const float alpha ) : r(color.r), g(color.g), b(color.b), a(alpha) {}  // remplace alpha.
    
    float r, g, b, a;
};

//! construit une couleur opaque.
Color make_color( const float r, const float g, const float b );
//! construit une couleur transparente.
Color make_alpha_color( const float r, const float g, const float b, const float a );
//! renvoie la couleur opaque (color.rgb, 1).
Color make_opaque_color( const Color& color );

//! utilitaire. renvoie une couleur noire.
Color Black( );
inline Color make_black( ) { return Black(); }

//! utilitaire. renvoie une couleur blanche.
Color White( );
inline Color make_white( ) { return White(); }

//! utilitaire. renvoie une couleur rouge.
Color Red( );
inline Color make_red( ) { return Red(); }

//! utilitaire. renvoie une couleur verte.
Color Green( );
inline Color make_green( ) { return Green(); }

//! utilitaire. renvoie une couleur bleue.
Color Blue( );
inline Color make_blue( ) { return Blue(); }


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
