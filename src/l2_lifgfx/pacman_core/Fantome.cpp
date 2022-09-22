#include "Fantome.h"
#include <stdlib.h>

Fantome::Fantome () {
	x = y = 10;
	dir = 0;
}

void Fantome::gauche (const Terrain & t) {
	if (t.estPositionPersoValide(x-1,y)) x--;
}

void Fantome::droite (const Terrain & t) {
	if (t.estPositionPersoValide(x+1,y)) x++;
}

void Fantome::haut (const Terrain & t) {
	if (t.estPositionPersoValide(x,y+1)) y++;
}

void Fantome::bas (const Terrain & t) {
	if (t.estPositionPersoValide(x,y-1)) y--;
}

int Fantome::getX () const { return x; }

int Fantome::getY () const { return y; }

void Fantome::versPacman (const Terrain & t, const Pacman & p) {
    int dx = p.getX()-x;
    int dy = p.getY()-y;
    if (dx>0) dx=1;
    if (dx<0) dx=-1;
    if (dy>0) dy=1;
    if (dy<0) dy=-1;
    if (t.estPositionPersoValide(x+dx,y+dy)) {
        x = x+dx;
        y = y+dy;
    }
}

void Fantome::bougeAuto (const Terrain & t) {
    int dx [4] = { 1, 0, -1, 0};
    int dy [4] = { 0, 1, 0, -1};
    int xtmp,ytmp;
    xtmp = x + dx[dir];
    ytmp = y + dy[dir];
    if (t.estPositionPersoValide(xtmp,ytmp)) {
        x = xtmp;
        y = ytmp;
    }
    else dir = rand()%4;
}
