/*
 * MSS.cpp : Systeme masses-ressorts
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


/** \file MSS.cpp
*  \brief Structures de donnees relatives au systeme masses-ressorts
*/



/** Librairies de base **/
#include <stdio.h>
#include <fstream>
#include <math.h> 

// Fichiers de gkit2light
#include "vec.h"

// Fichiers de master_meca_sim
#include "MSS.h"





/**
* Calcul de la longueur au repos d un ressort.
 */
void Ressort::SetLrepos()
{
    // Calcul de la longueur du ressort entre les deux particules :
    // Distance entre les particules A et B constituant le ressort
    // l(A,B) = sqrt( (xA-xB)2 + (yA-yB)2 + (zA-zB)2)
    _Spring->_L0 = length(GetParticuleA()->GetPosition() - GetParticuleB()->GetPosition());
}


/**
* Modification du systeme masses-ressorts : creation d une face (= 3 aretes).
 */
void MSS::MakeFace(Particule *p1, Particule *p2, Particule *p3, Spring *R)
{
	/* Une face est constituee de trois aretes */
	// Construction de l arete entre p1 et p2
	MakeEdge(p1, p2, R);
	
	// Construction de l arete entre p2 et p3
	MakeEdge(p2, p3, R);
	
	// Construction de l arete entre p3 et p1
	MakeEdge(p3, p1, R);
}


/**
 * Modification du systeme masses-ressorts : creation d une arete.
 */
void MSS::MakeEdge(Particule *p1, Particule *p2, Spring *R)
{
	// Verification que le ressort entre ces deux particules n existe pas deja
	bool exist = false;
	
	// Liste des ressorts relies a p1
	std::vector<Ressort *> ressortList = p1->GetRessortList();
	
	// Parcours de la liste des ressorts de p1
	for(int i=0; i<ressortList.size(); ++i)
    {
		// Regarde si il existe un ressort relie a p1 et p2
		if ( ((ressortList[i])->GetParticuleA() == p2) || ((ressortList[i])->GetParticuleB() == p2))
		{
			// Le ressort existe deja entre p1 et p2
			exist = true;
			
			// Arret du parcours 
			break;
		}
		
    }
	
	// Ajout d un nouveau ressort dans le systeme masses-ressorts
	if (!exist)
    {
		// Construction d un ressort entre les particules p1 et p2
		Ressort *res = new Ressort(p1, p2, R);
		
		// Ajout du ressort res dans la liste des ressorts de p1
		p1->AddRessort(res);
		
		// Ajout du ressort res dans la liste des ressorts de p2
		p2->AddRessort(res);
		
		// Ajout du ressort res dans la liste des ressorts du systeme masses-ressorts
		this->AddRessort(res);
    }
	
}


