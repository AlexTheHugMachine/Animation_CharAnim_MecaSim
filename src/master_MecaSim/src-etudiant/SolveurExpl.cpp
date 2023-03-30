/*
 * SolveurExpl.cpp : Application schemas semi-implicite sur les objets.
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

/** \file Calculs.cpp
 Fonctions de calculs communes aux objets simules.
 \brief Fonctions de calculs communes aux objets simules.
 */

#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>

#include "vec.h"
#include "ObjetSimule.h"
#include "Viewer.h"
#include "SolveurExpl.h"


using namespace std;



/**
 * Calcul de l acceleration des particules
 * avec ajout de la gravite aux forces des particules
 * et ajout de la force due au vent sur une des particules du maillage
 * et reinitialisation des forces.
 */
void SolveurExpl::CalculAccel_ForceGravite(Vector g,
                                           int nb_som,
                                           std::vector<Vector> &A,
                                           std::vector<Vector> &Force,
                                           std::vector<float> &M)
{
 


 			//// Cas SPH
            // On a calcule dans Force[i] : fij / rho_i
            // Il ne reste qu a ajoute le vecteur g
            // a_i = fij / rho_i + g

    for(int i=0; i<nb_som; i++)
    {
        Force[i].x = Force[i].x + M[i]*g.x;
        Force[i].y = Force[i].y + M[i]*g.y;
        Force[i].z = Force[i].z + M[i]*g.z;

        if(M[i] != 0.0)
        {
            A[i].x = Force[i].x/M[i];
            A[i].y = Force[i].y/M[i];
            A[i].z = Force[i].z/M[i];
        }
        else
        {
            A[i].x = 0.0;
            A[i].y = 0.0;
            A[i].z = 0.0;
        }

        Force[i].x = 0.0;
        Force[i].y = 0.0;
        Force[i].z = 0.0;
    } 

    
}//void


/*! Calcul des vitesses et positions : 
 *  Formule d Euler semi-implicite :
 *  x'(t+dt) = x'(t) + dt x"(t)
 *  x(t+dt) = x(t) + dt x'(t+dt)
 */
void SolveurExpl::Solve(float visco,
                        int nb_som,
                        int Tps,
                        std::vector<Vector> &A,
                        std::vector<Vector> &V,
                        std::vector<Vector> &P)
{
    
    for(int i=0; i<nb_som; i++)
    {
        V[i].x = V[i].x + _delta_t*A[i].x;
        V[i].y = V[i].y + _delta_t*A[i].y;
        V[i].z = V[i].z + _delta_t*A[i].z;

        P[i].x = P[i].x + _delta_t*V[i].x;
        P[i].y = P[i].y + _delta_t*V[i].y;
        P[i].z = P[i].z + _delta_t*V[i].z;
    }
}//void
