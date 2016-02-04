
#include <cassert>
#include <cstring>
#include <vector>

#include "window.h"
#include "widgets.h"


Widgets create_widgets( )
{
    Widgets w;
    w.console= create_text();
    w.px= 0; w.py= 0;
    w.focus= 0; w.fx= 0; w.fy= 0;
    w.px= 0; w.py= 0;
    return w;
}

void release_widgets( Widgets& w )
{
    release_text(w.console);
}


void begin( Widgets& w )
{
    clear(w.console);
    w.px= 0;
    w.py= 0;

    SDL_MouseButtonEvent mouse= button_event();
    w.mb= 0;
    w.mx= mouse.x / 8;
    w.my= mouse.y / 16;
    if(mouse.state == SDL_PRESSED)
    {
        clear_button_event();
        w.mb= 1;
    }

    SDL_KeyboardEvent key= key_event( );
    w.key= 0;
    w.mod= 0;
    if(key.type == SDL_KEYDOWN)
    {
        clear_key_event();
        w.key= key.keysym.sym;
        w.mod= key.keysym.mod;
        
        // filtre les touches speciales
        switch(w.key)
        {
            case SDLK_BACKSPACE:
            case SDLK_DELETE:
            case SDLK_LEFT:
            case SDLK_RIGHT:
            case SDLK_RETURN:
                break;
            default:
                w.key= 0;
        }
    }
    
    SDL_TextInputEvent input= text_event();
    if(input.text[0] != 0)
    {
        w.key= input.text[0];
        clear_text_event();
    }
}

struct Rect
{
    int x, y;
    int w, h;
};

static
bool overlap( const Rect r, const int x, const int y )
{
    return (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h);
}

static
Rect place( Widgets& w, const int px, const int py, const int width )
{
    Rect r;
    r.x= w.px;
    r.y= w.py;
    r.w= width;
    r.h= 1;
    
    w.px= r.x + r.w +2; // +2 marge
    if(py > 0)
    {
        // placement du widget sur une nouvelle ligne
        r.x= 0;
        r.y= r.y + 1;
        // placement du prochain widget a droite
        w.px= r.w +2;   // +2 marge
        w.py= r.y;
    }
    
    return r;
}

static
Rect place( Widgets& w, const int px, const int py, const char *text )
{
    return place(w, px, py, strlen(text));
}


void label( Widgets& w, const int px, const int py, const char *text )
{
    Rect r= place(w, px, py, text);
    print(w.console, r.x, r.y, text);
}

bool button( Widgets& w, const int px, const int py, const char *text, int& status )
{
    Rect r= place(w, px, py, strlen(text) +2);
    
    bool change= false;
    if(w.mb > 0 && overlap(r, w.mx, w.my))
    {
        change= true;
        status= (status + 1) % 2;
    }

    char tmp[128];
    if(status)
        snprintf(tmp, sizeof(tmp), "%c %s", 22, text);  // strlen(text) + 2
    else
        snprintf(tmp, sizeof(tmp), "%c %s", 20, text);  // strlen(text) + 2

    print(w.console, r.x, r.y, tmp);
    return change;
}

bool edit( Widgets& w, const int px, const int py, int text_size, char *text )
{
    assert(text_size > 1);
    int size= std::min((int) strlen(text), text_size -2);
    Rect r= place(w, px, py, text_size -1);
    
    // focus
    bool change= false;
    if(w.mb > 0)
    {
        if(overlap(r, w.mx, w.my))
            w.focus= 1;
        else
        {
            change= (w.focus > 0);      // click en dehors de la zone editable
            w.focus= 0;
        }
        
        w.fx= w.mx;
        w.fy= w.my;
        if(w.fx >= r.x + size)
            w.fx= r.x + size;
    }
    
    // edition
    if(w.focus > 0 && w.key > 0)
    {
        int c= w.fx - r.x;
        assert(c < text_size -1);
        
        if(w.key == SDLK_BACKSPACE)
        {
            w.fx--;     // curseur a gauche
            for(int i= c -1; i >= 0 && i+1 < text_size; i++) text[i]= text[i+1];
        }
        else if(w.key == SDLK_DELETE)
        {
            // curseur ne bouge pas
            for(int i= c; i+1 < text_size; i++) text[i]= text[i+1];
        }
        else if(w.key == SDLK_LEFT)
        {
            w.fx--;     // curseur a gauche
        }
        else if(w.key == SDLK_RIGHT)
        {
            w.fx++;     // curseur a droite
        }
        else if(w.key == SDLK_RETURN)
        {
            w.focus= 0;
            change= true;
        }
        else
        {
            w.fx++;     // curseur a droite
            for(int i= text_size -1; i > c; i--)
                text[i]= text[i -1];
            text[c]= w.key;

            if(size < text_size -2)
                size++;
            text[size]= 0;
        }
        
        // verifier que le curseur reste dans la zone editable
        if(w.fx < r.x)
            w.fx= r.x;
        if(w.fx >= r.x + size)
            w.fx= r.x + size;
    }
    
    int i= 0;
    char tmp[128];
    for(; text[i] != 0; i++) tmp[i]= text[i];   // copie les caracteres
    for(; i < text_size; i++) tmp[i]= ' ';      // complete avec des espaces
    tmp[text_size -1]= 0;                       // termine avec un 0

    print_background(w.console, r.x, r.y, tmp);
    if(w.focus > 0)
        print_background(w.console, w.fx, w.fy, 2, '_');
    
    return change;
}

void end( Widgets& w )
{
    return;
}

void draw( Widgets& w, const int width, const int height )
{
    draw(w.console, width, height);
}


