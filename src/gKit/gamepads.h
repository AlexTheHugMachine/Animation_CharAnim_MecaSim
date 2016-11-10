
#ifndef _GAMEPAD_H
#define _GAMEPAD_H

#include <SDL2/SDL.h>


struct Gamepad
{
    Gamepad( ) : m_pad(nullptr), m_state(0) {}
    Gamepad( SDL_GameController *pad ) : m_pad(pad), m_state(1) {}
    
    ~Gamepad( ) {}
    
    //! renvoie l'etat d'un bouton.
    int button( const SDL_GameControllerButton b );
    //! desactive un button.
    void clear_button( const SDL_GameControllerButton b );
    
    //! renvoie la position d'un axe.
    float axis( const SDL_GameControllerAxis a );
    //! re-initialise la position d'un axe.
    void clear_axis( const SDL_GameControllerAxis a );
    
    int m_buttons[SDL_CONTROLLER_BUTTON_MAX];
    float m_axis[SDL_CONTROLLER_AXIS_MAX];
    
    SDL_GameController *m_pad;
    int m_state;
};

struct Gamepads
{
    Gamepads( );
    ~Gamepads( );
    
    //!
    bool create( );
    void release( );
    
    //! 
    void update( );
    
    //! renvoie le nombre de game controllers.
    int pads( );
    //! renvoie un game controller.
    Gamepad& pad( const unsigned int index );

    //! renvoie l'etat d'un button d'un controlleur. cf la doc SDL2 pour les codes.
    int button( const unsigned int index, const SDL_GameControllerButton b );
    //! desactive un button d'un controlleur.
    void clear_button( const unsigned int index, const SDL_GameControllerButton b );

    //! renvoie la position d'un axe d'un controlleur.
    float axis( const unsigned int index, const SDL_GameControllerAxis a );
    //! re-initialise la position d'un axe d'un controlleur.
    void clear_axis( const unsigned int index, const SDL_GameControllerAxis a );
    
protected:
    std::vector<Gamepad> m_pads;
};

#endif
