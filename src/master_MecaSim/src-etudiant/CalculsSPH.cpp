/*
 * CalculsSPH.cpp :
 * Copyright (C) 2017 Florence Zara, LIRIS
 *               florence.zara@liris.univ-lyon1.fr
 *               http://liris.cnrs.fr/florence.zara/
 *
 * Utilisation du code :
 * https://www.cs.cornell.edu/~bindel/class/cs5220-f11/code/sph.pdf
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


/** \file CalculsSPH.cpp
Programme calculant pour  un fluide son etat au pas de temps suivant (methode d 'Euler semi-implicite) en utilisant la methode SPH (Smoothed Particles Hydrodynamics):
principales fonctions de calculs.  
\brief Fonctions de calculs pour un fluide avec methode SPH.
*/ 

#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>

#include "vec.h"
#include "ObjetSimule.h"
#include "ObjetSimuleSPH.h"
#include "Viewer.h"

using namespace std;



/**
 * Calcul des densites des particules.
 * Formule :
 *  \rho_i = \frac{4m}{\pi h^8} \sum_{j \in N_i} (h^2 - r^2)^3.
 */
void ObjetSimuleSPH::CalculDensite()
{
    
    
}//void


/**
 * Calcul des forces d interaction entre particules.
  * Attention - Calcul direct de fij / rho_i
 */
void ObjetSimuleSPH::CalculInteraction(float visco)
{
    
}//void


/**
 * Gestion des collisions.
 * Notre condition aux limites correspond à une frontière inélastique
 * avec un coefficient de restitution spécifié < 1. Quand
 * une particule frappe une barrière verticale ([[which = 0]]) ou horizontale
 * barrier ([[which = 1]]), nous le traitons avec [[damp_reflect]].
 * Cela réduit la distance totale parcourue en fonction du temps écoulé depuis
 * la collision reflète, amortit les vitesses, et reflète
 * quels que soient les composants de la solution à refléter.
 */

void ObjetSimuleSPH::damp_reflect(int frontiere, float barrier, int indice_part)
{
    /// frontiere : indique quelle frontiere (x, y, z) du domaine est concernee
    

}


/**
 * Gestion des collisions.
 * Pour chacune des particules nous verifions la reflection
 * avec chacun des 4 murs du domaine.
 */
void ObjetSimuleSPH::Collision()
{
   
    
}

