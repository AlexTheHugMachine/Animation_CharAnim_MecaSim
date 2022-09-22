/**
@brief Module gérant un Jeu (de Pacman)

@file Jeu.h
@date 2017/01/17
*/

#ifndef _JEU_H
#define _JEU_H

#include "Pacman.h"
#include "Terrain.h"
#include "Fantome.h"

/**
@brief Un jeu (de pacman) = un terrain et un pacman
*/
class Jeu {

private :

	Terrain ter;
	Pacman pac;
	Fantome fan;

public :

    Jeu ();

    Terrain& getTerrain ();
    Pacman& getPacman ();

    const Terrain& getConstTerrain () const;
    const Pacman& getConstPacman () const;
    const Fantome& getConstFantome () const;
    int getNombreDeFantome() const;

    void actionsAutomatiques ();
    void actionClavier(const char touche);

};

#endif
