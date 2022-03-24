
#include "text.h"
#include "app.h"

struct UI: public App
{
    UI( ) : App(1024, 640) {}
    
    int init( )
    {
        m_console=create_text();
        return 0;
    }
    
    int quit( )
    {
        release_text(m_console);
        return 0;
    }
    
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        clear(m_console);
        printf(m_console, 0, 0, "en haut a gauche");
        
        for(int i= 1; i < 10; i++)
            printf(m_console, 4, i, "ligne %d", i);
        
        // dessiner la console
        draw(m_console, window_width(), window_height());
        
        return 1;       // on continue
    }
    
    Text m_console;
};

int main( int argc, char **argv )
{
    UI app;
    app.run();
    
    return 0;
}
