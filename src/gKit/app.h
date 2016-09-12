
#include "window.h"

/*! classe application.

    tuto7.cpp et tuto8.cpp presentent un exemple simple d'utilisation.
 */
 
class App
{
public:
    //! constructeur, dimensions de la fenetre et version d'openGL.
    App( const int width, const int height, const int major= 3, const int minor= 3 );
    virtual ~App( );

    //! a deriver pour creer les objets openGL.
    virtual int init( ) = 0;
    //! a deriver pour detruire les objets openGL.
    virtual int quit( ) = 0;

    //! a deriver et redefinir pour animer les objets en fonction du temps.
    virtual int update( const float time, const float delta ) { return 0; }
    //! a deriver pour afficher les objets.
    virtual int render( ) = 0;
    
    //! execution de l'application.
    int run( );
    
    //! renvoie le temps ecoule depuis le lancement de l'application, en millisecondes.
    float global_time( );
    //! renvoie le temps ecoule depuis la derniere frame, en millisecondes.
    float delta_time( );

protected:
    Window m_window;
    Context m_context;

    unsigned int m_time;
    unsigned int m_delta;
};

