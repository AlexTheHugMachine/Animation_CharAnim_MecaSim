/*
 * CalculsParticule.cpp :
 * Copyright (C) 2016 Florence Zara, LIRIS
 *               florence.zara@liris.univ-lyon1.fr
 *               http://liris.cnrs.fr/florence.zara/
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/** \file CalculsParticule.cpp
Programme calculant pour chaque particule i son etat au pas de temps suivant (methode d 'Euler semi-implicite) :
principales fonctions de calculs.  
\brief Fonctions de calculs sur un ensemble de particules.
*/ 

#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>

#include "vec.h"
#include "ObjetSimule.h"
#include "ObjetSimuleParticule.h"
#include "Viewer.h"

using namespace std;





/**
 * Gestion des collisions avec le sol et bords de la fenetre - rebond.
 */
void ObjetSimuleParticule::Collision()
{
 
    // Collision juste par rapport au plan horizontal passant par le y
   
    
}

