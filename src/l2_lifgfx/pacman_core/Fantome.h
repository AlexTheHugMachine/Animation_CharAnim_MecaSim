/**
@brief Module gérant un fantome
@file Fantome.h
@date 2017/01/17
*/

#ifndef _FANTOME_H
#define _FANTOME_H

#include "Terrain.h"
#include "Pacman.h"

/**
@brief Un fantome = sa position 2D
*/
class Fantome {

private :

	int x,y;
	int dir;

public:

    Fantome();

    void gauche (const Terrain & t);
    void droite (const Terrain & t);
    void haut (const Terrain & t);
    void bas (const Terrain & t);

    int getX () const;
    int getY () const;

    void versPacman (const Terrain & t, const Pacman & p);
    void bougeAuto (const Terrain & t);

};

#endif
