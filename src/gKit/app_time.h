
#ifndef _APP_TIME_H
#define _APP_TIME_H

#include <chrono>

#include "glcore.h"
#include "app.h"
#include "text.h"


//! \addtogroup application

//! \file
//! classe application, avec mesure integree du temps d'execution cpu et gpu.
class AppTime : public App
{
public:
    //! constructeur, dimensions de la fenetre et version d'openGL.
    AppTime( const int width, const int height, const int major= 3, const int minor= 3 );
    virtual ~AppTime( );

    //! a deriver pour creer les objets openGL.
    virtual int init( ) = 0;
    //! a deriver pour detruire les objets openGL.
    virtual int quit( ) = 0;

    //! a deriver et redefinir pour animer les objets en fonction du temps.
    virtual int update( const float time, const float delta ) { return 0; }

    //! a deriver pour afficher les objets.
    virtual int render( ) = 0;

protected:
    virtual int prerender( );
    virtual int postrender( );

    std::chrono::high_resolution_clock::time_point m_cpu_start;
    std::chrono::high_resolution_clock::time_point m_cpu_stop;
    Text m_console;
    GLuint m_time_query;
};


#endif // _APP_H
