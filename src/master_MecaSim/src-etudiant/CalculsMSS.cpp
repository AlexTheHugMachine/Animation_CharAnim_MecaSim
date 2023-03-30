/*
 * CalculsMSS.cpp :
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

/** \file CalculsMSS.cpp
Programme calculant pour chaque particule i d un MSS son etat au pas de temps suivant 
 (methode d 'Euler semi-implicite) : principales fonctions de calculs.
\brief Fonctions de calculs de la methode semi-implicite sur un systeme masses-ressorts.
*/ 

#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>

#include "vec.h"
#include "ObjetSimule.h"
#include "ObjetSimuleMSS.h"
#include "Viewer.h"

using namespace std;





/**
* Calcul des forces appliquees sur les particules du systeme masses-ressorts.
 */
void ObjetSimuleMSS::CalculForceSpring()
{
	/// f = somme_i (ki * (l(i,j)-l_0(i,j)) * uij ) + (nuij * (vi - vj) * uij) + (m*g) + force_ext
	
	/// Rq : Les forces dues a la gravite et au vent sont ajoutees lors du calcul de l acceleration
    
	/// Calcul des forces de ressorts
	for(int i = 0; i < _Nb_Sommets; i++)
	{
		Force[i] = Vector(0.0, 0.0, 0.0);
	};

    std::vector<Ressort*> maillageRessorts = _SystemeMasseRessort->GetRessortList();
    for (int i = 0; i < maillageRessorts.size(); i++) {
        Ressort* ressort = maillageRessorts[i];
        Particule* pa = ressort->GetParticuleA();
        Particule* pb = ressort->GetParticuleB();

        float lressort = length(P[pa->GetId()] - P[pb->GetId()]);
        
        
        Vector fRaideur = ressort->GetRaideur() * (lressort - ressort->GetLrepos())* normalize(P[pa->GetId()] - P[pb->GetId()]);
        Vector fAmortis = ressort->GetFactAmorti() * cross((V[pa->GetId()] - V[pb->GetId()]), normalize(P[pa->GetId()] - P[pb->GetId()]));
        Force[pa->GetId()] = Force[pa->GetId()] - fRaideur - fAmortis;
        Force[pb->GetId()] = Force[pb->GetId()] + fRaideur + fAmortis;

		if((Force[pa->GetId()].x || Force[pa->GetId()].y || Force[pa->GetId()].z) >= 10.0)
		{
			ressort->SetRaideur(0.0);
			ressort->SetAmortiss(0.0);
		}
	}
		
}//void


/**
 * Gestion des collisions avec le sol.
 */
void ObjetSimuleMSS::Collision()
{
    /// Arret de la vitesse quand touche le plan
    for (int i = 0; i < _Nb_Sommets; i++)
	{
		if (P[i].y < -10.0)
		{
			V[i].y = 0.0;
		}
	}
    
}// void

