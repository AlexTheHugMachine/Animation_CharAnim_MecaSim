/**
@brief Module gérant un Pacman

@file Pacman.h
@date 2017/01/17
*/

#ifndef _PACMAN_H
#define _PACMAN_H

#include "Terrain.h"

/**
@brief Un Pacman = sa position 2D
*/
class Pacman {

private :

	int x,y;

public:

    Pacman ();

    void gauche (const Terrain & t);
    void droite (const Terrain & t);
    void haut (const Terrain & t);
    void bas (const Terrain & t);

    int getX () const;
    int getY () const;

};

#endif
