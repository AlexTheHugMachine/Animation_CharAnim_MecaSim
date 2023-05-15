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

	Vector Vec_vent = Vector(2.0, 5.0, 0.0);

	for(int i = 0; i < _Nb_Sommets; i++)
	{
		Force[i] = Vector(0.0, 0.0, 0.0);
		Force[i] = Force[i] + Vec_vent;
	};

    std::vector<Ressort*> maillageRessorts = _SystemeMasseRessort->GetRessortList();
    for (int i = 0; i < maillageRessorts.size(); i++) {
        Ressort* ressort = maillageRessorts[i];
        Particule* pa = ressort->GetParticuleA();
        Particule* pb = ressort->GetParticuleB();

		if(ressort->GetRaideur() != 0.0 && ressort->GetAmortissement() != 0.0) {
			float lressort = length(P[pa->GetId()] - P[pb->GetId()]);
			if(lressort > 2.0) {
				ressort->SetRaideur(0.0);
				ressort->SetAmortiss(0.0);
			}
			
        	Vector fRaideur = ressort->GetRaideur() * (lressort - ressort->GetLrepos())* normalize(P[pa->GetId()] - P[pb->GetId()]);
        	Vector fAmortis = ressort->GetFactAmorti() * cross((V[pa->GetId()] - V[pb->GetId()]), normalize(P[pa->GetId()] - P[pb->GetId()]));
        	Force[pa->GetId()] = Force[pa->GetId()] - fRaideur - fAmortis;
        	Force[pb->GetId()] = Force[pb->GetId()] + fRaideur + fAmortis;
		}
	}
}//void


/**
 * Gestion des collisions avec le sol.
 */
void ObjetSimuleMSS::Collision()
{
	float rayon = 5.0f;
    //Vector center = Vector(10, -5, 0); // centre de la sphere côté
	Vector center = Vector(10, -5, 0);

    /// Arret de la vitesse quand touche le plan
    for (int i = 0; i < _Nb_Sommets; i++)
	{
		if (P[i].y < -10.0)
		{
			V[i].y = 0.0;
		}
	}

	/// Collision avec un mesh
	for(int i = 0; i < _Nb_Sommets; i++)
	{
		if (P[i].x < 6.4 && P[i].x > 0.9 && P[i].z > 0 && P[i].z < 10 && P[i].y < -3.2 && P[i].y > -10)
		{
			V[i] = Vector(0.0, 0.0, 0.0);
			P[i] = P[i] - Vector(0.01, 0.01, 0.01);
		}
	}

	/// Collision avec une sphere
	for (int i = 0; i < _Nb_Sommets; i++) 
	{
		if (length(center - P[i]) <= rayon) {
            P[i] = center + (P[i] - center) * (rayon / length(P[i] - center) * 1.01);
            V[i] = 0;
            A[i] = 0;
        }
	}
	
    
}// void

