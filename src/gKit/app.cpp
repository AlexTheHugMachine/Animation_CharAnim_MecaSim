
#include "app.h"
#include "glcore.h"


App::App( const int width, const int height, const int major, const int minor )
    : m_window(nullptr), m_context(nullptr)
{
    m_window= create_window(width, height);
    m_context= create_context(m_window, major, minor);
}

App::~App( )
{
    if(m_context) 
        release_context(m_context);
    if(m_window) 
        release_window(m_window);
}

int App::run( )
{
    if(init() < 0)
        return -1;
    
    // configure openGL
    glViewport(0, 0, window_width(), window_height());
    
    unsigned int base= SDL_GetTicks();
    while(events(m_window))
    {
        unsigned int delta= SDL_GetTicks() - base;
        if(update((float) base, (float) delta) < 0)
            break;
        if(render() < 0)
            break;
        
        base= SDL_GetTicks();
        
        // presenter le resultat
        SDL_GL_SwapWindow(m_window);
    }
    
    if(quit() < 0)
        return -1;
    return 0;    
}
