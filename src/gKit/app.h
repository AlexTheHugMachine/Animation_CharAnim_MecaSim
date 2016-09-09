
#include "window.h"


class App
{
public:
    App( const int width, const int height, const int major= 3, const int minor= 3 );
    virtual ~App( );

    virtual int init( ) = 0;
    virtual int quit( ) = 0;

    virtual int update( const float time, const float delta ) { return 0; }
    virtual int draw( ) = 0;

    int run( );

protected:
    Window m_window;
    Context m_context;
};

