#include "Pacman.h"

Pacman::Pacman () {
	x = y = 1;
}

void Pacman::gauche (const Terrain & t) {
	if (t.estPositionPersoValide(x-1,y)) x--;
}

void Pacman::droite (const Terrain & t) {
	if (t.estPositionPersoValide(x+1,y)) x++;
}

void Pacman::haut (const Terrain & t) {
	if (t.estPositionPersoValide(x,y+1)) y++;
}

void Pacman::bas(const Terrain & t) {
	if (t.estPositionPersoValide(x,y-1)) y--;
}

int Pacman::getX () const { return x; }

int Pacman::getY () const { return y; }
